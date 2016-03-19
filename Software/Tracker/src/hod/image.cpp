#include "image.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "fl_math.h"

void image_init(image* img, int w, int h) {
  img->width = w;
  img->height = h;
  img->len = w * h;
  img->pitch = w * sizeof(float);
  img->bytes = img->pitch * h;
  img->data = (float*)calloc(w * h, sizeof(float));
}

void image_destroy(image* img) {
  free(img->data);
  img->data = NULL;
}

void image_clear(image* img, float v) {
  for (int i = 0; i < img->len; i++) {
    img->data[i] = v;
  }
}

void image_map_values(image* img, float a, float b, float c, float d) {
  for (int i = 0; i < img->len; i++) {
    float v = img->data[i];
    img->data[i] = fl_map_range(v, a, b, c, d);
  }
}

void copy_sub_image_scale(const image* src, const image_region* src_reg,
                              image* dst, const image_region* dst_reg) {
  const float x_ratio = float(src_reg->width) / float(dst_reg->width);
  const float y_ratio = float(src_reg->height) / float(dst_reg->height);

  for (int i = 0; i < dst_reg->height; i++) {
    int y = int(y_ratio * i);
    for (int j = 0; j < dst_reg->width; j++) {
      int x = int(x_ratio * j);
      int src_index = (src_reg->y + y) * src->width + (src_reg->x + x);

      int dst_offset = (i + dst_reg->y) * dst->width + (j + dst_reg->x);

      dst->data[dst_offset++] = src->data[src_index];
    }
  }
}

void copy_sub_image(const image* src, const image_region* src_reg, image* dst,
                    const image_region* dst_reg) {
  for (int y = 0; y < dst_reg->height; y++) {
    for (int x = 0; x < dst_reg->width; x++) {
      int src_index = (src_reg->y + y) * src->width + (src_reg->x + x);
      int dst_offset = (dst_reg->y + y) * dst->width + (dst_reg->x + x);
      dst->data[dst_offset++] = src->data[src_index];
    }
  }
}
