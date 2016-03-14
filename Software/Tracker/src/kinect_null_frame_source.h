#ifndef KINECT_NULL_FRAME_SOURCE_H
#define KINECT_NULL_FRAME_SOURCE_H

#include "kinect_frame_source.h"

struct kinect_null_frame_source : kinect_frame_source {
  const kinect_frame* get_frame() override { return NULL; }
};

#endif
