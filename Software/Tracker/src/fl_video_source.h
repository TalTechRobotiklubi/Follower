#pragma once

#include "kinect_frame_source.h"
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include "kinect_frame.h"

struct fl_video_source : kinect_frame_source {
  fl_video_source(const char* file);
  ~fl_video_source();
  const kinect_frame* get_frame(float dt) override;

  float current_frame_offset = 0.f;
  float fps;
  std::vector<uint8_t> rgba_frame;
  kinect_frame kframe;
  cv::VideoCapture capture;
};
