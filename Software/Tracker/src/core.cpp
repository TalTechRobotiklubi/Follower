#include "core.h"
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
  c->udp = UdpHostCreate("127.0.0.1", 9001);

  if (!c->udp) {
    fprintf(stderr, "Failed to create UDP host.\n");
    abort();
  }

  c->kinect_frame_thread = std::thread(kinect_loop, c);
}

void core_serialize(core* c) {
  c->builder.Clear();
  auto depth = c->builder.CreateVector(c->encoded_depth.data, c->encoded_depth.len);
  auto frame = proto::CreateFrame(c->builder, depth);
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

    c.frame_source->fill_frame(&c.current_frame);
    memcpy(c.prev_rgba_depth.data, c.rgba_depth.data, c.rgba_depth.bytes);
    depth_to_rgba(c.current_frame.depth_data, c.current_frame.depth_length,
                  &c.rgba_depth);
    BlockDiff(c.prev_rgba_depth.data, c.rgba_depth.data, kDepthWidth,
              kDeptHeight, &c.rgba_depth_diff);
    c.encoded_depth =
        EncodeImage(c.encoder, c.rgba_depth.data, &c.rgba_depth_diff);

    core_serialize(&c);

    printf("Frame: %d\n", c.current_frame.depth_length);
    printf("ms: %f\n", frame_time);

    UdpHostPoll(c.udp);

    c.serial.receive(&c.in_data);
    AlgorithmRunner::run(0, c.in_data, &c.out_data);
    DL_task(loopTimeMs);
    c.serial.send(c.out_data);
    waitTillLoopTimeElapses();
  }

  return 0;
}
