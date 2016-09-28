#include <stdio.h>
#include <string.h>
#include "core.h"
#include "kinect_frame.h"
#include "kinect_frame_source.h"
#include "core_opt.h"

void kinect_loop(core* c) {
  while (c->running) {
    c->frame_source->get_frame();
    printf("New frame\n");
  }
}

void core_start(core* c) {
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
  while (c.running) {
    c.frame_source->fill_frame(c.current_frame);
    printf("Frame: %d\n", c.current_frame->depth_length);
  }

  return 0;
}
