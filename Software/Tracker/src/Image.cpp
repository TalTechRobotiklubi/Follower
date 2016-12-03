#include "Image.h"
#include <assert.h>
#include <stdlib.h>
#include "FlMath.h"

static uint8_t DepthToByte(uint16_t value, uint16_t min, uint16_t max) {
  const uint32_t v = Clamp(value, min, max);
  const uint32_t lmin = min;
  const uint32_t lmax = max;
  return uint8_t((v - lmin) * 255 / (lmax - lmin));
}

void RgbaImageInit(RgbaImage* img, int w, int h) {
  img->width = w;
  img->height = h;
  img->channels = 4;
  img->stride = w * img->channels;
  img->bytes = img->stride * h;
  img->data = (uint8_t*)calloc(img->bytes, 1);
}

void DepthToRgba(const uint16_t* depth, int len, RgbaImage* img) {
  assert(img->width * img->height == len);
  for (int i = 0; i < len; i++) {
    const int idx = 4 * i;
    uint8_t* data = img->data;
    uint8_t v = DepthToByte(depth[i], 500, 4500);
    data[idx] = v;
    data[idx + 1] = v;
    data[idx + 2] = v;
    data[idx + 3] = 255;
  }
}
