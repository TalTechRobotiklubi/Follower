#include "kinect_frame.h"
#include <stdlib.h>
#include <string.h>

void copy_frame(const kinect_frame* src, kinect_frame* dst) {
  if (src->depth_length != dst->depth_length) {
    if (dst->depth_data) {
      free(dst->depth_data);
    }
    dst->depth_data = (uint16_t*)calloc(src->depth_length, sizeof(uint16_t));
  }

  dst->depth_length = src->depth_length;

  if (src->rgba_length != dst->rgba_length) {
    if (dst->rgba_data) {
      free(dst->rgba_data);
    }
    dst->rgba_data = (uint8_t*)calloc(src->rgba_length, 1);
  }

  dst->rgba_width = src->rgba_width;
  dst->rgba_height = src->rgba_height;
  dst->rgba_length = src->rgba_length;

  memcpy(dst->depth_data, src->depth_data, src->depth_length);
  memcpy(dst->rgba_data, src->rgba_data, src->rgba_length);
}
