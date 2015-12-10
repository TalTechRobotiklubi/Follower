#ifndef KINECT_FRAME_SOURCE_H
#define KINECT_FRAME_SOURCE_H

struct kinect_frame;

struct kinect_frame_source {
  virtual const kinect_frame* get_frame(float dt) = 0;
  virtual ~kinect_frame_source() {}
};

#endif
