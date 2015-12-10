#ifndef KINECT_FRAME_H
#define KINECT_FRAME_H

#include <stdint.h>
#include "AABB.h"

struct kinect_body {
  AABB body_box;
  uint64_t identifier;
};

struct kinect_frame {
  uint16_t* depth_data = nullptr;
  size_t depth_length = 0;

  uint16_t* infrared_data = nullptr;
  size_t infrared_length = 0;

  int rgba_width = 0;
  int rgba_height = 0;
  uint8_t* rgba_data = nullptr;
  size_t rgba_length = 0;

  kinect_body* bodies = nullptr;
  size_t num_bodies = 0;
};

#endif
