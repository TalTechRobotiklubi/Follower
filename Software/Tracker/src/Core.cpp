#include "Core.h"
#include <fhd.h>
#include <fhd_kinect.h>
#include <libyuv.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <lua.hpp>
#include "Clock.h"
#include "Constants.h"
#include "CoreOptions.h"
#include "Encode.h"
#include "File.h"
#include "KinectFrameSource.h"
#include "Message.h"
#include "SqliteFrameWriter.h"
#include "UdpHost.h"
#include "comm/datalayer.h"
#include "png/lodepng.h"
#include "proto/message_generated.h"

void CopyRgbaSubImage(const KinectFrame* frame, int x, int y, int w, int h,
                      RgbaImage* dst) {
  const int stride = frame->rgbaWidth * 4;
  const int offset = y * stride + x * 4;
  libyuv::ARGBScale(&frame->rgbaData[offset], stride, w, h, dst->data,
                    dst->stride, dst->width, dst->height, libyuv::kFilterNone);
}

void CoreDecide(Core* c, double dt) {
  ScriptLoaderUpdate(&c->scripts, dt, c->world, &c->state, &c->tracking);
}

void CoreSendStatusMessage(Core* c, const char* message) {
  flatbuffers::FlatBufferBuilder builder;
  auto content = builder.CreateString(message);
  auto m = proto::CreateMessage(
      builder, proto::Payload_StatusMessage,
      proto::CreateStatusMessage(builder, content).Union());
  builder.Finish(m);
  UdpHostBroadcast(c->udp, builder.GetBufferPointer(), builder.GetSize(), true);
}

void CoreStart(Core* c) {
  ScriptLoaderSetLogCallback(
      &c->scripts,
      [](const char* s, void* user) { CoreSendStatusMessage((Core*)user, s); },
      c);

  auto kinectLoop = [](Core* core) {
    for (;;) {
      core->frameSource->GetFrame();
    }
  };
  c->kinectFrameThread = std::thread(kinectLoop, c);

  c->classifiers.erase(
      std::remove_if(c->classifiers.begin(), c->classifiers.end(),
                     [](const std::unique_ptr<Classifier>& classifier) {
                       return !classifier->Init();
                     }),
      c->classifiers.end());
}

void CoreStopActions(Core* c) {
  ScriptLoaderExec(&c->scripts, "decide = nil");
  c->state = ControlState();
}

void CoreHandleCommand(Core* c, const proto::Command* command) {
  switch (command->type()) {
    case proto::CommandType_Stop:
      CoreStopActions(c);
      CoreSendStatusMessage(c, kMsgOk);
      break;
    case proto::CommandType_Speed: {
      c->state.speed = float(atof(command->arg()->c_str()));
      std::string actual = std::to_string(c->state.speed);
      CoreSendStatusMessage(c, actual.c_str());
      break;
    }
    case proto::CommandType_RotationSpeed: {
      c->state.rotationSpeed = float(atof(command->arg()->c_str()));
      std::string actual = std::to_string(c->state.rotationSpeed);
      CoreSendStatusMessage(c, actual.c_str());
      break;
    }
    case proto::CommandType_StopVideo: {
      c->sendVideo = false;
      CoreSendStatusMessage(c, kMsgOk);
      break;
    }
    case proto::CommandType_StartVideo: {
      c->sendVideo = true;
      CoreSendStatusMessage(c, kMsgOk);
      break;
    }
    case proto::CommandType_RecordDepth: {
      if (!c->writer) {
        const char* db = "depth_frames.db";
        c->writer = SqliteFrameWriterCreate(db);
        CoreSendStatusMessage(c, "recording to depth_frames.db");
      } else {
        CoreSendStatusMessage(c, "already recording");
      }
      break;
    }
    case proto::CommandType_StopRecord: {
      if (c->writer) {
        SqliteFrameWriterDestroy(c->writer);
        c->writer = nullptr;
      }
      CoreSendStatusMessage(c, kMsgOk);
      break;
    }
    case proto::CommandType_StartDebug: {
      c->sendDebugData = true;
      CoreSendStatusMessage(c, kMsgOk);
      break;
    }
    case proto::CommandType_StopDebug: {
      c->sendDebugData = false;
      CoreSendStatusMessage(c, kMsgOk);
      break;
    }
    default: {
      CoreSendStatusMessage(c, kMsgUnknownCommand);
      break;
    }
  }
}

void CoreHandleMessage(Core* c, const uint8_t* data, size_t) {
  auto message = proto::GetMessage(data);
  if (!message) {
    return;
  }

  switch (message->payload_type()) {
    case proto::Payload_LuaMainScript: {
      auto scriptMessage = (const proto::LuaMainScript*)message->payload();
      const char* remoteScript = scriptMessage->content()->c_str();

      if (ScriptLoaderExec(&c->scripts, remoteScript)) {
        CoreSendStatusMessage(c, "load successful");
      } else {
        const char* err = ScriptLoaderGetError(&c->scripts);
        if (err) {
          printf("failed to load wrapper script: %s\n", err);
          CoreSendStatusMessage(c, err);
        } else {
          CoreSendStatusMessage(c, "failed to load script, no lua error");
        }
      }
      break;
    }
    case proto::Payload_Command: {
      auto command = (const proto::Command*)message->payload();
      CoreHandleCommand(c, command);
      break;
    }
    default:
      break;
  }
}

void CoreDetect(Core* c, double timestamp) {
  World* world = c->world;
  world->timestamp = timestamp;
  world->numDetections = 0;

  fhd_run_pass(c->fhd, c->kinectFrame.depthData);

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
      Vec2i tl{kDepthWidth - candidate->depth_position.x -
                   candidate->depth_position.width,
               candidate->depth_position.y};
      Vec2i br{tl.x + candidate->depth_position.width,
               tl.y + candidate->depth_position.height};
      Vec3 metricPos{candidate->metric_position.x, candidate->metric_position.y,
                     candidate->metric_position.z};
      Detection* d = &world->detections[world->numDetections];
      d->depthTopLeft = tl;
      d->depthBotRight = br;
      d->metricPosition = metricPos;
      d->weight = candidate->weight;
      CopyRgbaSubImage(&c->kinectFrame, candidate->depth_position.x,
                       candidate->depth_position.y,
                       candidate->depth_position.width,
                       candidate->depth_position.height, &d->color);

      memset(d->histogram, 0, sizeof(d->histogram));

      for (int i = 0; i < d->color.bytes; i += 4) {
        uint8_t r = d->color.data[i];
        uint8_t g = d->color.data[i + 1];
        uint8_t b = d->color.data[i + 2];
        d->histogram[r] += 1.f;
        d->histogram[256 + g] += 1.f;
        d->histogram[512 + b] += 1.f;
      }

      world->numDetections++;
    }
  }
}

void CoreSerialSend(Core* c) {
  c->outData.translation_speed = int16_t(c->state.speed);
  c->outData.rotation_speed = int16_t(c->state.rotationSpeed);
  c->outData.camera_degrees = c->state.camera;
  c->outData.activity = (Activity)c->tracking.activity;
  c->serial.send(c->outData);
}

void CoreSerialize(Core* c) {
  c->builder.Clear();
  std::vector<flatbuffers::Offset<proto::Detection>> detections;
  std::vector<proto::Target> targets;
  detections.reserve(c->world->numDetections);
  targets.reserve(c->tracking.numTargets);

  bool debugging = c->sendDebugData;

  for (int32_t i = 0; i < c->world->numDetections; i++) {
    const Detection& detection = c->world->detections[i];

    proto::Vec2i tl(detection.depthTopLeft.x, detection.depthTopLeft.y);
    proto::Vec2i br(detection.depthBotRight.x, detection.depthBotRight.y);
    proto::Vec3 metric(detection.metricPosition.x, detection.metricPosition.y,
                       detection.metricPosition.z);

    if (debugging) {
      unsigned char* png = nullptr;
      size_t pngSize = 0;
      unsigned res =
          lodepng_encode32(&png, &pngSize, detection.color.data,
                           detection.color.width, detection.color.height);

      detections.push_back(proto::CreateDetection(
          c->builder, &tl, &br, &metric, detection.weight,
          c->builder.CreateVector(
              detection.histogram,
              sizeof(detection.histogram) / sizeof(detection.histogram[0])),
          res ? 0 : c->builder.CreateVector(png, pngSize)));

      free(png);
    } else {
      detections.push_back(proto::CreateDetection(c->builder, &tl, &br, &metric,
                                                  detection.weight));
    }
  }

  for (int32_t i = 0; i < c->tracking.numTargets; i++) {
    const Target& t = c->tracking.targets[i];
    targets.push_back(
        proto::Target(t.weight, proto::Vec2(t.kinect.x, t.kinect.y),
                      proto::Vec3(t.position.x, t.position.y, t.position.z)));
  }

  auto detectionOffsets = c->builder.CreateVector(detections);
  auto targetOffsets = c->builder.CreateVectorOfStructs(targets);
  auto tracking = proto::CreateTrackingState(
      c->builder, c->tracking.activeTarget, targetOffsets);

  proto::Vec2 camera(c->state.camera.x, c->state.camera.y);
  auto depth =
      c->sendVideo
          ? c->builder.CreateVector(c->encodedDepth.data, c->encodedDepth.len)
          : 0;
  auto frame = proto::CreateFrame(c->builder, c->timestamp, c->dtMilli, &camera,
                                  c->state.rotationSpeed, c->state.speed, depth,
                                  detectionOffsets, tracking);
  auto message =
      proto::CreateMessage(c->builder, proto::Payload_Frame, frame.Union());
  c->builder.Finish(message);
}

Core::Core()
    : world((World*)calloc(1, sizeof(World))),
      encoder(EncoderCreate(kDepthWidth, kDeptHeight)),
      fhd((fhd_context*)calloc(1, sizeof(fhd_context))) {
  KinectFrameInit(&kinectFrame, kDepthWidth, kDeptHeight);
  RgbaImageInit(&rgbaDepth, kDepthWidth, kDeptHeight);
  RgbaImageInit(&prevRgbaDepth, kDepthWidth, kDeptHeight);
  ActiveMapReset(&rgbaDepthDiff, kDepthWidth, kDeptHeight);

  fhd_context_init(fhd, kDepthWidth, kDeptHeight, 8, 8);

  for (Detection& d : world->detections) {
    RgbaImageInit(&d.color, kCandidateWidth, kCandidateHeight);
  }
}

Core::~Core() {
  if (kinectFrameThread.joinable()) kinectFrameThread.join();
  if (writer) SqliteFrameWriterDestroy(writer);
}

int main(int argc, char** argv) {
  Core c;
  if (!ScriptLoaderInit(&c.scripts)) {
    printf("Failed to initialize lua\n");
    return 1;
  }

  if (!ParseCoreOptions(&c, argc, argv)) {
    return 1;
  }

  CoreStart(&c);

  double currentTime = ms_now();
  double prevTime = currentTime;
  const double broadcastInterval = 0.04;
  double timeUntilBroadCast = broadcastInterval;
  for (;;) {
    prevTime = currentTime;
    currentTime = ms_now();
    const double frameTime = currentTime - prevTime;
    const double frameTimeSeconds = frameTime / 1000.0;
    c.timestamp += frameTime;
    c.dtMilli = frameTime;

    int sourceFrameNum = c.frameSource->FrameNumber();
    if (sourceFrameNum != c.frameNum) {
      c.frameNum = sourceFrameNum;

      c.frameSource->FillFrame(&c.kinectFrame);

      if (c.writer) {
        SqliteFrameWriterAddFrame(c.writer, &c.kinectFrame);
      }
    }

    if (c.sendVideo && timeUntilBroadCast <= 0.0) {
      memcpy(c.prevRgbaDepth.data, c.rgbaDepth.data, c.rgbaDepth.bytes);
      DepthToRgba(c.kinectFrame.depthData, c.kinectFrame.depthLength,
                  &c.rgbaDepth);
      BlockDiff(c.prevRgbaDepth.data, c.rgbaDepth.data, kDepthWidth,
                kDeptHeight, &c.rgbaDepthDiff);
      c.encodedDepth =
          EncodeImage(c.encoder, c.rgbaDepth.data, &c.rgbaDepthDiff);
    }

    c.serial.receive(&c.inData);
    memcpy(c.world->distanceSensors, &c.inData,
           sizeof(uint8_t) * NUM_OF_DISTANCE_SENSORS);

    CoreDetect(&c, currentTime);
    CoreDecide(&c, frameTimeSeconds);

    DL_task(int16_t(frameTime));
    CoreSerialSend(&c);

    const IoVec* received = UdpHostPoll(c.udp);

    if (received) {
      CoreHandleMessage(&c, received->data, received->len);
    }

    if (timeUntilBroadCast <= 0.0) {
      CoreSerialize(&c);
      UdpHostBroadcast(c.udp, c.builder.GetBufferPointer(), c.builder.GetSize(),
                       false);
      timeUntilBroadCast = broadcastInterval;
    } else {
      timeUntilBroadCast -= frameTimeSeconds;
    }
  }

  return 0;
}
