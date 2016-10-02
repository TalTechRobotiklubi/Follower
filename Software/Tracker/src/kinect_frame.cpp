#include "kinect_frame.h"
#include <string.h>
#include <assert.h>

void copy_frame(const kinect_frame* src, kinect_frame* dst) {
  assert(src->depth_length == dst->depth_length);

  if (src->depth_data)  {
    memcpy(dst->depth_data, src->depth_data, src->depth_length);
  }

  if (src->rgba_data) {
    memcpy(dst->rgba_data, src->rgba_data, src->rgba_length);
  }
}
