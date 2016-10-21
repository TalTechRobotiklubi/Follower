#include "core.h"
#include <cmath>
#include <fhd.h>
#include <stdio.h>
#include <string.h>
#include "Clock.h"
#include "Encode.h"
#include "UdpHost.h"
#include "algorithm/algorithm_runner.h"
#include "comm/datalayer.h"
#include "core_opt.h"
#include "fl_constants.h"
#include "kinect_frame.h"
#include "kinect_frame_source.h"
#include "proto/frame_generated.h"

void kinect_loop(core* c) {
  while (c->running) {
    c->frame_source->get_frame();
  }
}

void core_start(core* c) {
  c->kinect_frame_thread = std::thread(kinect_loop, c);
}

void core_check(core* c, double dt) {
  if (c->timestamp > 1000.0 * 5.0) {
    c->coreState = kFind;
    return;
  }

  c->state.camera.x = 45.f * float(std::sin(c->timestamp / 100.0));
}

void core_decide(core* c, double dt) {
  std::vector<Target>& targets = c->tracking.targets;
  for (Target& t : targets) {
    t.timeToLive -= dt;
  }

  targets.erase(
      std::remove_if(targets.begin(), targets.end(),
                     [](const Target& t) { return t.timeToLive <= 0.0; }),
      targets.end());

  if (c->world.numDetections == 0) {
    return;
  }

  const std::vector<Target> prevTargets = targets;
  for (const Detection& d : c->world.detections) {
    if (d.weight < 1.f) continue;

    if (prevTargets.empty()) {
      targets.emplace_back(3.0, d.kinectPosition, d.metricPosition);
    } else {
      // Find the closest target
      float minDist = 10000.f;
      size_t minIdx = 0;
      for (size_t i = 0; i < prevTargets.size(); i++) {
        const Target& t = prevTargets[i];
        const float dist =
            vec2_distance(t.metricPosition.xz(), d.metricPosition.xz());
        if (dist < minDist) {
          minIdx = i;
          minDist = dist;
        }
      }

      targets[minIdx].kinectPosition = d.kinectPosition;
      targets[minIdx].metricPosition = d.metricPosition;
      targets[minIdx].timeToLive = 3.0;
    }
  }

  if (!targets.empty()) {
    const Target& t = targets.front();
    const float deg = (std::atan2(t.kinectPosition.y - 424.f,
                             t.kinectPosition.x - 512.f * 0.5f) + F_PI_2) * 180.f / F_PI;
    const float degDiff = 2.f;
    if (deg < -5.f) {
      c->state.camera.x += degDiff;
    } else if (deg > 5.f) {
      c->state.camera.x -= degDiff;
    }
  }
}

void core_detect(core* c, double timestamp) {
  if (!c->fhd) {
    return;
  }

  fhd_run_pass(c->fhd, c->current_frame.depth_data);

  c->world.timestamp = timestamp;
  c->world.numDetections = 0;

  // Flip the detection horizontally, Kinect 2's images have left-right reversed.
  const float w = float(kDepthWidth);
  for (int i = 0; i < std::min(c->fhd->candidates_len, kMaxDetections); i++) {
    const fhd_candidate* candidate = &c->fhd->candidates[i];
    vec2 kinectPos{w - candidate->kinect_position.x, candidate->kinect_position.y};
    vec3 metricPos{candidate->metric_position.x, candidate->metric_position.y,
                   candidate->metric_position.z};
    c->world.detections[i] = Detection{kinectPos, metricPos, candidate->weight};
    c->world.numDetections++;
  }
}

void core_serial_send(core* c) {
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
  targets.reserve(c->tracking.targets.size());

  for (const Detection& detection : c->world.detections) {
    detections.push_back(proto::Detection(
        proto::Vec2(detection.kinectPosition.x, detection.kinectPosition.y),
        proto::Vec3(detection.metricPosition.x, detection.metricPosition.y,
                    detection.metricPosition.z),
        detection.weight));
  }

  for (const Target& t : c->tracking.targets) {
    targets.push_back(proto::Target(
      float(t.timeToLive),
      proto::Vec2(t.kinectPosition.x, t.kinectPosition.y),
      proto::Vec3(t.metricPosition.x, t.metricPosition.y, t.metricPosition.z)
    ));
  }

  auto detectionOffsets = c->builder.CreateVectorOfStructs(detections);
  auto targetOffsets = c->builder.CreateVectorOfStructs(targets);

  proto::Vec2 camera(c->state.camera.x, c->state.camera.y);
  proto::FrameBuilder frame_builder(c->builder);

  frame_builder.add_timestamp(c->timestamp);
  frame_builder.add_camera(&camera);
  frame_builder.add_depth(depth);
  frame_builder.add_detections(detectionOffsets);
  frame_builder.add_targets(targetOffsets);

  auto frame = frame_builder.Finish();
  c->builder.Finish(frame);
}

core::~core() { kinect_frame_thread.join(); }

int main(int argc, char** argv) {
  core c;
  c.current_frame.depth_data = (uint16_t*)calloc(kDepthWidth * kDeptHeight, 2);
  c.current_frame.depth_length = kDepthWidth * kDeptHeight;
  rgba_image_init(&c.rgba_depth, kDepthWidth, kDeptHeight);
  rgba_image_init(&c.prev_rgba_depth, kDepthWidth, kDeptHeight);
  ActiveMapReset(&c.rgba_depth_diff, kDepthWidth, kDeptHeight);

  c.encoder = EncoderCreate(kDepthWidth, kDeptHeight);

  if (!parse_opt(&c, argc, argv)) {
    return 1;
  }

  core_start(&c);

  double current_time = ms_now();
  double prev_time = current_time;
  while (c.running) {
    prev_time = current_time;
    current_time = ms_now();
    const double frame_time = current_time - prev_time;
    const double frameTimeSeconds = frame_time / 1000.0;
    c.timestamp += frame_time;


    c.frame_source->fill_frame(&c.current_frame);
    memcpy(c.prev_rgba_depth.data, c.rgba_depth.data, c.rgba_depth.bytes);

    core_detect(&c, current_time);

    depth_to_rgba(c.current_frame.depth_data, c.current_frame.depth_length,
                  &c.rgba_depth);
    BlockDiff(c.prev_rgba_depth.data, c.rgba_depth.data, kDepthWidth,
              kDeptHeight, &c.rgba_depth_diff);
    c.encoded_depth =
        EncodeImage(c.encoder, c.rgba_depth.data, &c.rgba_depth_diff);

    c.serial.receive(&c.in_data);

    switch (c.coreState) {
      case kCheck:
        core_check(&c, frameTimeSeconds);
        break;
      case kFind: {
        core_decide(&c, frameTimeSeconds);
        break;
      }
      default:
        break;
    }

    DL_task(int16_t(frame_time));
    core_serial_send(&c);
    core_serialize(&c);

    UdpHostPoll(c.udp);
    UdpHostBroadcast(c.udp, c.builder.GetBufferPointer(), c.builder.GetSize());
  }

  return 0;
}
