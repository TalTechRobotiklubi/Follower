#include <stdio.h>
#include <string.h>
#include "core.h"
#include "comm/DataLayer.h"
#include "kinect_frame.h"
#include "kinect_frame_source.h"
#include "core_opt.h"
#include "UdpHost.h"
#include "timer.h"
#include "algorithm/algorithm_runner.h"

typedef std::chrono::milliseconds msec;
static int loopTimeMs = 30;

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

void waitTillLoopTimeElapses()
{
  static auto loop_time = std::chrono::system_clock::now();

  while (std::chrono::duration_cast<msec>(
           std::chrono::system_clock::now() - loop_time).count() < loopTimeMs)
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  loop_time = std::chrono::system_clock::now();
}

int main(int argc, char** argv) {

  core c;
  c.current_frame = (kinect_frame*)calloc(1, sizeof(kinect_frame));

  if (!parse_opt(&c, argc, argv)) {
    return 1;
  }

  core_start(&c);

  while (c.running) {
    c.frame_source->fill_frame(c.current_frame);
    printf("Frame: %d\n", c.current_frame->depth_length);

    UdpHostPoll(c.udp);

    c.serial.receive(&c.in_data);
    AlgorithmRunner::run(0, c.in_data, &c.out_data);
    DL_task(loopTimeMs);
    c.serial.send(c.out_data);
    waitTillLoopTimeElapses();
  }

  return 0;
}
