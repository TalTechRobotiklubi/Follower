#include "core.h"
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

typedef std::chrono::milliseconds msec;
static int loopTimeMs = 30;

void kinect_loop(core* c) {
  while (c->running) {
    c->frame_source->get_frame();
  }
}

void core_start(core* c) {
  c->kinect_frame_thread = std::thread(kinect_loop, c);
}

void core_decide(core* c) {
  if (!c->fhd) return;

  const fhd_candidate* closest = nullptr;
  for (int i = 0; i < c->fhd->candidates_len; i++) {
    const fhd_candidate* candidate = &c->fhd->candidates[i];
    if (candidate->weight >= 1.f) {
      if (!closest ||
          closest->metric_position.z > candidate->metric_position.z) {
        closest = candidate;
      }
    }
  }

  if (closest) {
    const float deg = (atan2(closest->kinect_position.y - 424.f,
                             closest->kinect_position.x - 512.f * 0.5f) +
                       F_PI_2) *
                      180.f / F_PI;
    c->state.camera.x = deg;
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
  if (c->fhd) {
    detections.resize(c->fhd->candidates_len);
    for (int i = 0; i < c->fhd->candidates_len; i++) {
      const fhd_candidate* candidate = &c->fhd->candidates[i];
      detections[i] =
          proto::Detection(proto::Vec2(candidate->kinect_position.x,
                                       candidate->kinect_position.y),
                           proto::Vec3(candidate->metric_position.x,
                                       candidate->metric_position.y,
                                       candidate->metric_position.z),
                           candidate->weight);
    }
  }
  auto detectionOffsets = c->builder.CreateVectorOfStructs(detections);
  proto::Vec2 camera(c->state.camera.x, c->state.camera.y);
  proto::FrameBuilder frame_builder(c->builder);
  frame_builder.add_timestamp(c->timestamp);
  frame_builder.add_camera(&camera);
  frame_builder.add_depth(depth);
  frame_builder.add_detections(detectionOffsets);
  auto frame = frame_builder.Finish();
  c->builder.Finish(frame);
}

core::~core() { kinect_frame_thread.join(); }

void waitTillLoopTimeElapses() {
  static auto loop_time = std::chrono::system_clock::now();

  while (std::chrono::duration_cast<msec>(std::chrono::system_clock::now() -
                                          loop_time)
             .count() < loopTimeMs)
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  loop_time = std::chrono::system_clock::now();
}

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
    c.timestamp += frame_time;

    c.frame_source->fill_frame(&c.current_frame);
    memcpy(c.prev_rgba_depth.data, c.rgba_depth.data, c.rgba_depth.bytes);

    if (c.fhd) {
      fhd_run_pass(c.fhd, c.current_frame.depth_data);
    }

    depth_to_rgba(c.current_frame.depth_data, c.current_frame.depth_length,
                  &c.rgba_depth);
    BlockDiff(c.prev_rgba_depth.data, c.rgba_depth.data, kDepthWidth,
              kDeptHeight, &c.rgba_depth_diff);
    c.encoded_depth =
        EncodeImage(c.encoder, c.rgba_depth.data, &c.rgba_depth_diff);


    c.serial.receive(&c.in_data);
    core_decide(&c);

    DL_task(loopTimeMs);
    core_serial_send(&c);
    core_serialize(&c);

    UdpHostPoll(c.udp);
    UdpHostBroadcast(c.udp, c.builder.GetBufferPointer(), c.builder.GetSize());

    waitTillLoopTimeElapses();
  }

  return 0;
}
