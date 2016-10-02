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
  printf("%p\n", depth);
  printf("%d %d %d\n", img->width, img->height, len);
  assert(img->width * img->height == len);
  for (int i = 0; i < len; i++) {
    uint16_t reading = depth[i];
    uint8_t normalized = depth_to_byte(reading, 500, 4500);

    int idx = 4 * i;

    uint8_t* data = img->data;
    if (reading >= 500 && reading <= 4500) {
      data[idx] = normalized;
      data[idx + 1] = 255 - normalized;
      data[idx + 2] = 255 - normalized;
    } else {
      data[idx] = 0;
      data[idx + 1] = 0;
      data[idx + 2] = 0;
    }
    data[idx + 3] = 255;
  }
}
