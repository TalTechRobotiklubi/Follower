#ifndef FL_KINECT_STREAM_SOURCE
#define FL_KINECT_STREAM_SOURCE

#include "kinect_frame_source.h"

struct fl_kinect_stream_source : kinect_frame_source {
  fl_kinect_stream_source(const char* file);
  ~fl_kinect_stream_source();
  const kinect_frame* get_frame(float dt) override;

  struct fl_stream_reader* stream_reader = nullptr;
};

#endif
