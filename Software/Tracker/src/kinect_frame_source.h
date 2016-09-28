#ifndef KINECT_FRAME_SOURCE_H
#define KINECT_FRAME_SOURCE_H

struct kinect_frame;

struct kinect_frame_source {
  virtual const kinect_frame* get_frame() = 0;
  virtual void fill_frame(kinect_frame* dst) = 0;
  virtual void advance() {}
  virtual ~kinect_frame_source() {}
};

#endif
