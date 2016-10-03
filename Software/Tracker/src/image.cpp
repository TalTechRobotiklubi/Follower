#include "image.h"
#include "fl_math.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static uint8_t depth_to_byte(uint16_t value, uint16_t min, uint16_t max) {
  const uint32_t v = fl_clamp(value, min, max);
  const uint32_t lmin = min;
  const uint32_t lmax = max;
  return uint8_t((v - lmin) * 255 / (lmax - lmin));
}

void rgba_image_init(rgba_image* img, int w, int h) {
  img->width = w;
  img->height = h;
  img->channels = 4;
  img->pitch = w * img->channels;
  img->bytes = img->pitch * h;
  img->data = (uint8_t*)calloc(img->bytes, 1);
}

void depth_to_rgba(const uint16_t* depth, int len, rgba_image* img) {
  assert(img->width * img->height == len);
  for (int i = 0; i < len; i++) {
    const int idx = 4 * i;
    uint8_t* data = img->data;
    uint8_t v = depth_to_byte(depth[i], 500, 4500);
    data[idx] = v;
    data[idx + 1] = v;
    data[idx + 2] = v;
    data[idx + 3] = 255;
  }
}
