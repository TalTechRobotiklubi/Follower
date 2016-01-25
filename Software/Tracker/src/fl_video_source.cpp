#include "fl_video_source.h"
#include <stdio.h>
#include <assert.h>

fl_video_source::fl_video_source(const char* file)
  : capture(file) {

  const int width = int(capture.get(CV_CAP_PROP_FRAME_WIDTH));
  const int height = int(capture.get(CV_CAP_PROP_FRAME_HEIGHT));
  assert(width > 0 && height > 0);

  fps = float(capture.get(CV_CAP_PROP_FPS));

  rgba_frame.resize(width * height * 4);

  kframe.rgba_width = width;
  kframe.rgba_height = height;
  kframe.rgba_data = rgba_frame.data();
  kframe.rgba_length = rgba_frame.size();

}

fl_video_source::~fl_video_source() {

}

const kinect_frame* fl_video_source::get_frame(float dt) {
  if (!capture.isOpened()) return nullptr;

  current_frame_offset += dt;

  if (current_frame_offset < 1.f / fps) {
    return &kframe;
  } else {
    current_frame_offset = 0.f;
  }

  const int next_frame = int(capture.get(CV_CAP_PROP_POS_FRAMES));
  const int total_frames = int(capture.get(CV_CAP_PROP_FRAME_COUNT));

  if (next_frame == total_frames) {
    capture.set(CV_CAP_PROP_POS_AVI_RATIO, 0);
  }

  cv::Mat frame;
  capture >> frame;

  cv::Mat rgba_view(frame.size(), CV_8UC4, rgba_frame.data());
  cv::cvtColor(frame, rgba_view, CV_BGR2BGRA, 4);

  kframe.rgba_data = rgba_frame.data();
  kframe.rgba_length = rgba_frame.size();

  return &kframe;
}
