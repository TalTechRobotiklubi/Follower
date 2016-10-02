#pragma once

#include <stdint.h>

struct rgba_image {
  int width;
  int height;
  int channels;
  int pitch;
  int bytes;
  uint8_t* data;
};

void rgba_image_init(rgba_image* img, int w, int h);
void depth_to_rgba(const uint16_t* depth, int len, rgba_image* img);
