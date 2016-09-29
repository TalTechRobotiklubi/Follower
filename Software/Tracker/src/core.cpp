#include <stdio.h>
#include <string.h>
#include "core.h"
#include "comm/datahandler.h"
#include "kinect_frame.h"
#include "kinect_frame_source.h"
#include "core_opt.h"
#include "UdpHost.h"
#include "timer.h"
#include "algorithm/algorithm_runner.h"

void kinect_loop(core* c) {
  while (c->running) {
    c->frame_source->get_frame();
    printf("New frame\n");
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

core::~core() {
  kinect_frame_thread.join();
}

int main(int argc, char** argv) {

  core c;
  c.current_frame = (kinect_frame*)calloc(1, sizeof(kinect_frame));

  if (!parse_opt(&c, argc, argv)) {
    return 1;
  }

  core_start(&c);

  int64_t current_time = hp_counter();
  int64_t prev_time = current_time;
  while (c.running) {
    prev_time = current_time;
    current_time = hp_counter();
    const int64_t frame_time = current_time - prev_time;
    const double freq = double(hp_freq());
    const double dt = double(frame_time) * 1000.0 / freq;  // ms
    const double dt_seconds = dt / 1000.0f;
    c.frame_source->fill_frame(c.current_frame);
    printf("Frame: %d\n", c.current_frame->depth_length);

    UdpHostPoll(c.udp);

    c.serial.receive(&c.in_data);
    AlgorithmRunner::run(0, c.in_data, &c.out_data);
    DataHandler_TASK(dt);
    c.serial.send(c.out_data);
  }

  return 0;
}
