#ifndef FOLLOWER_CTX_H
#define FOLLOWER_CTX_H

#include <cstdint>
#include <array>
#include <memory>
#include <unordered_map>
#include <vector>
#include "AABB.h"
#include "AsyncSerial.h"
#include "kinect_frame.h"
#include "filter/kalman.h"
#include "filter/particle_filter.h"
#include <random>

const uint32_t range_intervals = 16;
const int KINECT_DEPTH_W = 512;
const int KINECT_DEPTH_H = 424;

const uint16_t MIN_RELIABLE_DIST = 500;
const uint16_t MAX_RELIABLE_DIST = 4500;

typedef std::array<uint16_t, range_intervals> depth_window;

struct kinect_frame_source;

struct body {
  float time_to_live;
  kinect_body kbody;
  std::vector<AABB> prev_positions;
  std::vector<vec2> prev_kalmans;
  kalman_filter filter;
};

struct follower_ctx {

  std::unique_ptr<CallbackAsyncSerial> serial;
	std::unique_ptr<kinect_frame_source> frame_source;
  std::unordered_map<uint64_t, body> bodies;

  struct hog_detect* hog = nullptr;
  std::vector<AABB> hog_boxes;

  // offset of depth calculation
  // depth window height = depth_height - (depth_height * depth_cutoff)
	float depth_cutoff = 0.8f;

  // distance in pixels that is required to latch a body from kinect frame
  // to previous bodies, if this distance is exceeded, a new body is added
  float min_body_latch_distance = 50.f;

  float body_time_to_live = 1.f;

	depth_window depth_map;

  ~follower_ctx();
};

void follower_kinect_update(follower_ctx* follower, const kinect_frame* frame);
void follower_update(follower_ctx* follower, float dt);

depth_window calculate_range_map(const uint16_t* depth_data,
                                 uint32_t w,
                                 uint32_t h,
                                 float img_height_offset = 0.8f);

bool follower_begin_serial(follower_ctx* follower, const std::string& port, uint32_t baud);
bool follower_has_serial(const follower_ctx* follower);
void send_serial_message(follower_ctx* follower, const uint8_t* data, size_t len);

#endif
