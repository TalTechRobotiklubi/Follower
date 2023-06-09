#include "core.h"
#include <fhd.h>
#include <fhd_kinect.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <lua.hpp>
#include "Clock.h"
#include "Encode.h"
#include "File.h"
#include "KinectFrameSource.h"
#include "UdpHost.h"
#include "comm/datalayer.h"
#include "core_opt.h"
#include "fl_constants.h"
#include "fl_sqlite_writer.h"
#include "proto/message_generated.h"

void kinect_loop(core* c) {
  while (c->running) {
    c->frameSource->GetFrame();
  }
}

void core_decide(core* c, double dt) {
  ScriptLoaderUpdate(&c->scripts, dt, &c->world, &c->state, &c->tracking);
}

void core_send_status_message(core* c, const char* message) {
  flatbuffers::FlatBufferBuilder builder;
  auto content = builder.CreateString(message);
  auto m = proto::CreateMessage(
      builder, proto::Payload_StatusMessage,
      proto::CreateStatusMessage(builder, content).Union());
  builder.Finish(m);
  UdpHostBroadcast(c->udp, builder.GetBufferPointer(), builder.GetSize(), true);
}

void core_start(core* c) {
	ScriptLoaderSetLogCallback(&c->scripts, [](const char* s, void* user) {
		core_send_status_message((core*)user, s);
	}, c);

	c->kinect_frame_thread = std::thread(kinect_loop, c);

	c->classifiers.erase(
		std::remove_if(c->classifiers.begin(), c->classifiers.end(),
			[](const std::unique_ptr<Classifier>& classifier) {
		return !classifier->Init();
	}),
		c->classifiers.end());
}

void core_stop_actions(core* c) {
  ScriptLoaderExec(&c->scripts, "decide = nil");
  c->state = ControlState();
}

void core_handle_command(core* c, const proto::Command* command) {
  switch (command->type()) {
    case proto::CommandType_Stop:
      core_stop_actions(c);
      core_send_status_message(c, "stop done");
      break;
    case proto::CommandType_Speed: {
      c->state.speed = float(atof(command->arg()->c_str()));
      std::string actual = std::to_string(c->state.speed);
      core_send_status_message(c, actual.c_str());
      break;
    }
    case proto::CommandType_RotationSpeed: {
      c->state.rotationSpeed = float(atof(command->arg()->c_str()));
      std::string actual = std::to_string(c->state.rotationSpeed);
      core_send_status_message(c, actual.c_str());
      break;
    }
    case proto::CommandType_StopVideo: {
      c->sendVideo = false;
      core_send_status_message(c, "ok");
      break;
    }
    case proto::CommandType_StartVideo: {
      c->sendVideo = true;
      core_send_status_message(c, "ok");
      break;
    }
    case proto::CommandType_RecordDepth: {
      if (!c->writer) {
        const char* db = "depth_frames.db";
        c->writer = fl_sqlite_writer_create(db);
        core_send_status_message(c, "recording to depth_frames.db");
      } else {
        core_send_status_message(c, "already recording");
      }
      break;
    }
    case proto::CommandType_StopRecord: {
      if (c->writer) {
        fl_sqlite_writer_destroy(c->writer);
        c->writer = nullptr;
      }
      core_send_status_message(c, "ok");
      break;
    }
    default: {
      core_send_status_message(c, "unknown command");
      break;
    }
  }
}

void core_handle_message(core* c, const uint8_t* data, size_t) {
  auto message = proto::GetMessage(data);
  if (!message) {
    return;
  }

  switch (message->payload_type()) {
    case proto::Payload_LuaMainScript: {
      auto scriptMessage = (const proto::LuaMainScript*)message->payload();
      const char* remoteScript = scriptMessage->content()->c_str();

      if (ScriptLoaderExec(&c->scripts, remoteScript)) {
        core_send_status_message(c, "load successful");
      } else {
        const char* err = ScriptLoaderGetError(&c->scripts);
        if (err) {
          printf("failed to load wrapper script: %s\n", err);
          core_send_status_message(c, err);
        } else {
          core_send_status_message(c, "failed to load script, no lua error");
        }
      }
      break;
    }
    case proto::Payload_Command: {
      auto command = (const proto::Command*)message->payload();
      core_handle_command(c, command);
      break;
    }
    case proto::Payload_Classifier: {
      core_send_status_message(c, "unsupported operation");
      break;
    }
    default:
      break;
  }
}

void core_detect(core* c, double timestamp) {
  c->world.timestamp = timestamp;
  c->world.numDetections = 0;

  fhd_run_pass(c->fhd, c->kinectFrame.depthData);


  const float w = float(kDepthWidth);
  size_t numCandidates = size_t(c->fhd->candidates_len);
  const size_t requiredPasses = std::max<size_t>(c->classifiers.size(), 1);


  for (size_t i = 0; i < numCandidates; i++) {
    const fhd_candidate* candidate = &c->fhd->candidates[i];

    size_t passed = 0;
    for (const std::unique_ptr<Classifier>& classifier : c->classifiers) {
      if (classifier->Classify(candidate)) {
        passed++;
      }
    }

    if (passed >= requiredPasses) {
      vec2 kinectPos{w - candidate->kinect_position.x,
                     candidate->kinect_position.y};
      vec3 metricPos{candidate->metric_position.x, candidate->metric_position.y,
                     candidate->metric_position.z};
      c->world.detections[c->world.numDetections] =
          Detection{kinectPos, metricPos, candidate->weight};
      c->world.numDetections++;
    }
  }
}

void core_serial_send(core* c) {
  c->out_data.translation_speed = int16_t(c->state.speed);
  c->out_data.rotation_speed = int16_t(c->state.rotationSpeed);
  c->out_data.camera_degrees = c->state.camera;
  c->serial.send(c->out_data);
}

void core_serialize(core* c) {
  c->builder.Clear();
  auto depth =
      c->builder.CreateVector(c->encoded_depth.data, c->encoded_depth.len);
  std::vector<proto::Detection> detections;
  std::vector<proto::Target> targets;
  detections.reserve(c->world.numDetections);
  targets.reserve(c->tracking.numTargets);

  for (int32_t i = 0; i < c->world.numDetections; i++) {
    const Detection& detection = c->world.detections[i];
    detections.push_back(proto::Detection(
        proto::Vec2(detection.kinectPosition.x, detection.kinectPosition.y),
        proto::Vec3(detection.metricPosition.x, detection.metricPosition.y,
                    detection.metricPosition.z),
        detection.weight));
  }

  for (int32_t i = 0; i < c->tracking.numTargets; i++) {
    const Target& t = c->tracking.targets[i];
    targets.push_back(
        proto::Target(t.weight, proto::Vec2(t.kinect.x, t.kinect.y),
                      proto::Vec3(t.position.x, t.position.y, t.position.z)));
  }

  auto detectionOffsets = c->builder.CreateVectorOfStructs(detections);
  auto targetOffsets = c->builder.CreateVectorOfStructs(targets);
  auto tracking = proto::CreateTrackingState(
      c->builder, c->tracking.activeTarget, targetOffsets);

  proto::Vec2 camera(c->state.camera.x, c->state.camera.y);

  proto::FrameBuilder frame_builder(c->builder);

  frame_builder.add_timestamp(c->timestamp);
  frame_builder.add_coreDtMs(c->dtMilli);
  frame_builder.add_camera(&camera);
  frame_builder.add_rotationSpeed(c->state.rotationSpeed);
  frame_builder.add_speed(c->state.speed);
  if (c->sendVideo) {
    frame_builder.add_depth(depth);
  }
  frame_builder.add_detections(detectionOffsets);
  frame_builder.add_tracking(tracking);

  auto frame = frame_builder.Finish();
  auto message =
      proto::CreateMessage(c->builder, proto::Payload_Frame, frame.Union());
  c->builder.Finish(message);
}

core::~core() {
  if (kinect_frame_thread.joinable()) kinect_frame_thread.join();
  if (writer) fl_sqlite_writer_destroy(writer);
}

int main(int argc, char** argv) {
  core c;
  if (!ScriptLoaderInit(&c.scripts)) {
    printf("Failed to initialize lua\n");
    return 1;
  }

  c.kinectFrame.depthData = (uint16_t*)calloc(kDepthWidth * kDeptHeight, 2);
  c.kinectFrame.depthLength = kDepthWidth * kDeptHeight;
  rgba_image_init(&c.rgba_depth, kDepthWidth, kDeptHeight);
  rgba_image_init(&c.prev_rgba_depth, kDepthWidth, kDeptHeight);
  ActiveMapReset(&c.rgba_depth_diff, kDepthWidth, kDeptHeight);

  c.encoder = EncoderCreate(kDepthWidth, kDeptHeight);
  c.fhd = (fhd_context*)calloc(1, sizeof(fhd_context));
  fhd_context_init(c.fhd, kDepthWidth, kDeptHeight, 8, 8);

  if (!parse_opt(&c, argc, argv)) {
    return 1;
  }

  core_start(&c);

  double current_time = ms_now();
  double prev_time = current_time;
  const double broadcastInterval = 0.03;
  double timeUntilBroadCast = broadcastInterval;
  while (c.running) {
    prev_time = current_time;
    current_time = ms_now();
    const double frame_time = current_time - prev_time;
    const double frameTimeSeconds = frame_time / 1000.0;
    c.timestamp += frame_time;
    c.dtMilli = float(frame_time);

    c.frameSource->FillFrame(&c.kinectFrame);

    if (c.writer) {
      fl_sqlite_writer_add_frame(c.writer, &c.kinectFrame);
    }

    memcpy(c.prev_rgba_depth.data, c.rgba_depth.data, c.rgba_depth.bytes);

    core_detect(&c, current_time);

    if (c.sendVideo && timeUntilBroadCast <= 0.0) {
      depth_to_rgba(c.kinectFrame.depthData, c.kinectFrame.depthLength,
                    &c.rgba_depth);
      BlockDiff(c.prev_rgba_depth.data, c.rgba_depth.data, kDepthWidth,
                kDeptHeight, &c.rgba_depth_diff);
      c.encoded_depth =
          EncodeImage(c.encoder, c.rgba_depth.data, &c.rgba_depth_diff);
    }

    c.serial.receive(&c.in_data);
    memcpy(c.world.distance_sensors, &c.in_data, sizeof(uint8_t) * NUM_OF_DISTANCE_SENSORS);
    core_decide(&c, frameTimeSeconds);

    DL_task(int16_t(frame_time));
    core_serial_send(&c);
    core_serialize(&c);

    const IoVec* received = UdpHostPoll(c.udp);

    if (received) {
      core_handle_message(&c, received->data, received->len);
    }

    if (timeUntilBroadCast <= 0.0) {
      UdpHostBroadcast(c.udp, c.builder.GetBufferPointer(),
                       c.builder.GetSize(), false);
      timeUntilBroadCast = broadcastInterval;
    } else {
      timeUntilBroadCast -= frameTimeSeconds;
    }
  }

  return 0;
}
