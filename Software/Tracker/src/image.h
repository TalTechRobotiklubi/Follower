#pragma once

#include <stdint.h>

struct rgba_image {
  int width = 0;
  int height = 0;
  int channels = 0;
  int pitch = 0;
  int bytes = 0;
  uint8_t* data = nullptr;
};

void rgba_image_init(rgba_image* img, int w, int h);
void depth_to_rgba(const uint16_t* depth, int len, rgba_image* img);
