#ifndef FOLLOWER_CTX_H
#define FOLLOWER_CTX_H

#include <cstdint>
#include <array>
#include <memory>
#include <unordered_map>
#include <vector>
#include "AABB.h"
#include "kinect_frame_source.h"
#include "kinect_frame.h"
#include <random>
#include <bgfx/bgfx.h>
#include "fl_constants.h"
#include "comm/serial_comm.h"
#include "comm/comm_output.h"
#include "hog_detect.h"
#include <opencv2/video/tracking.hpp>

const uint16_t MIN_RELIABLE_DIST = 500;
const uint16_t MAX_RELIABLE_DIST = 4500;

typedef std::array<uint16_t, fl::DEPTH_RANGE_INTERVALS> depth_window;

struct merged_aabb {
  AABB aabb;
  int num_aabbs;
};

struct body {
  float time_to_live;
  kinect_body kbody;
  std::vector<AABB> prev_positions;
  std::vector<vec2> prev_kalmans;
};

struct body_target {
  AABB location;
  float time_to_live;

  cv::KalmanFilter kf;
  cv::Mat_<float> z_t;
  cv::Mat_<float> x_t;
  cv::Mat_<float> w_k; 

  body_target();
  void update_kalman(const AABB* aabb);

  AABB kf_prediction;
};

struct follower_ctx {
  
  follower_ctx();
  ~follower_ctx();

  bgfx::TextureHandle depth_texture;
  bgfx::TextureHandle color_texture;
  bgfx::TextureHandle infrared_texture;

  SerialComm serial;
  std::unique_ptr<kinect_frame_source> frame_source;
  std::unordered_map<uint64_t, body> bodies;

  std::unique_ptr<hog_detect> hog;
  std::vector<AABB> hog_boxes;
  std::vector<merged_aabb> combined_hogs;

  bool has_target = false;
  body_target possible_target;

  // offset of depth calculation
  // depth window height = depth_height - (depth_height * depth_cutoff)
  float depth_cutoff = 0.8f;

  // distance in pixels that is required to latch a body from kinect frame
  // to previous bodies, if this distance is exceeded, a new body is added
  float min_body_latch_distance = 50.f;

  float body_time_to_live = 1.f;
  float downscale_quality = 0.f;

  depth_window depth_map;

  CommOutput out_data;

};

void follower_update(follower_ctx* follower, float dt);

depth_window calculate_range_map(const uint16_t* depth_data, uint32_t w,
                                 uint32_t h, float img_height_offset = 0.8f);

void follower_update_possible_position(follower_ctx* follower, const AABB* detections, size_t len);

#endif
