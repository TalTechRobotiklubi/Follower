#pragma once

#include <stdint.h>

struct RgbaImage {
  int width = 0;
  int height = 0;
  int channels = 0;
  int stride = 0;
  int bytes = 0;
  uint8_t* data = nullptr;
};

void RgbaImageInit(RgbaImage* img, int w, int h);
void DepthToRgba(const uint16_t* depth, int len, RgbaImage* img);
