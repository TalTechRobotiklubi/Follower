#ifndef KINECT_LIVE_SOURCE_H
#define KINECT_LIVE_SOURCE_H

#include "kinect_frame_source.h"
#include "kinect_frame.h"
#include <vector>
#include <mutex>
#include <cstdint>

struct IKinectSensor;
struct IDepthFrameReader;
struct IColorFrameReader;
struct fl_yuy2_convert;

struct kinect_live_source : kinect_frame_source {
  kinect_live_source();
  ~kinect_live_source();

  const kinect_frame* get_frame() override;

  IKinectSensor* kinect = nullptr;
  IDepthFrameReader* depth_reader = nullptr;
  IColorFrameReader* color_reader = nullptr;

  fl_yuy2_convert* yuy2_converter = nullptr;
  std::vector<uint16_t> depth_buffer;

  uint8_t* kinect_rgba_buf = nullptr;
  size_t kinect_rgba_buf_len = 0;

  kinect_frame current_frame;
  std::mutex frame_lock;
};

#endif
