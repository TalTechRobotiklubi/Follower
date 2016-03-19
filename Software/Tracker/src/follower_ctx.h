#ifndef FOLLOWER_CTX_H
#define FOLLOWER_CTX_H

#include <cstdint>
#include <array>
#include <memory>
#include <vector>
#include "AABB.h"
#include "kinect_frame_source.h"
#include "kinect_frame.h"
#include "fl_constants.h"
#include "comm/serial_comm.h"
#include "comm/comm_output.h"
#include "hod/hod.h"
#include "hod/hod_debug.h"

const uint16_t MIN_RELIABLE_DIST = 500;
const uint16_t MAX_RELIABLE_DIST = 4500;

typedef std::array<uint16_t, fl::DEPTH_RANGE_INTERVALS> depth_window;

struct body_target {
  float time_to_live = 1.f;

  int num_positions = 0;
  const int max_positions = 6;
  AABB positions[6];
  AABB avg_position;

  void update(const AABB* aabb, float dt);
};

struct follower_ctx {
  
  follower_ctx();
  ~follower_ctx();

  SerialComm serial;
  std::unique_ptr<kinect_frame_source> frame_source;

  bool has_target = false;
  body_target target;

  // offset of depth calculation
  // depth window height = depth_height - (depth_height * depth_cutoff)
  float depth_cutoff = 0.8f;

  depth_window depth_map;

  CommInput in_data;
  CommOutput out_data;

  hod_ctx hod;
  hod_debug* hod_debugger;
};

void follower_update(follower_ctx* follower, float dt);
void follower_update_candidates(follower_ctx* follower, float dt);

depth_window calculate_range_map(const uint16_t* depth_data, uint32_t w,
                                 uint32_t h, float img_height_offset = 0.8f);

#endif
