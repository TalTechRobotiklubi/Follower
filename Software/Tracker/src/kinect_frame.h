#pragma once
#include <stdint.h>

struct kinect_frame {
  int depth_length = 0;
  uint16_t* depth_data = nullptr;

  int rgba_width = 0;
  int rgba_height = 0;
  int rgba_length = 0;
  uint8_t* rgba_data = nullptr;
};

void copy_frame(const kinect_frame* src, kinect_frame* dst);
