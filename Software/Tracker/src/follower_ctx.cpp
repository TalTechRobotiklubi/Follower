#include "follower_ctx.h"
#include "hog_detect.h"
#include "fl_math.h"

void follower_update_camera_pos(follower_ctx* follower, const AABB* target) {
  const float target_x =
      (target->top_left.x + target->bot_right.x) * 0.5f / 512.f;
  const float target_y =
      (target->top_left.y + target->bot_right.y) * 0.5f / 424.f;

  follower->camera_degrees.x = fl_map_range(target_x, 0.f, 1.0f, -45.f, 45.f);
  follower->camera_degrees.y = fl_map_range(target_y, 0.f, 1.0f, 45.f, -45.f);
}

std::vector<merged_aabb> aabb_combine_overlapping(const AABB* aabb,
                                                  size_t len) {
  std::vector<merged_aabb> parent_nodes;
  std::vector<size_t> overlap_indices;

  for (size_t i = 0; i < len; i++) {
    const AABB* cur_aabb = &aabb[i];

    overlap_indices.clear();
    for (size_t j = 0; j < parent_nodes.size(); j++) {
      merged_aabb* parent_node = &parent_nodes[j];
      if (aabb_overlap(cur_aabb, &parent_node->aabb)) {
        overlap_indices.push_back(j);
      }
    }

    merged_aabb merge_node;
    merge_node.aabb = *cur_aabb;
    merge_node.num_aabbs = 1;

    if (overlap_indices.size() > 0) {
      for (size_t overlap_idx : overlap_indices) {
        merged_aabb* n = &parent_nodes[overlap_idx];
        aabb_expand(&merge_node.aabb, n->aabb.top_left);
        aabb_expand(&merge_node.aabb, n->aabb.bot_right);
        merge_node.num_aabbs += n->num_aabbs;
      }

      std::sort(overlap_indices.begin(), overlap_indices.end());

      for (int j = overlap_indices.size() - 1; j >= 0; j--) {
        parent_nodes.erase(parent_nodes.begin() + j);
      }
    }

    parent_nodes.push_back(merge_node);
  }

  return parent_nodes;
}

void follower_kinect_update(follower_ctx* follower, const kinect_frame* frame) {
  for (size_t i = 0; i < frame->num_bodies; i++) {
    const kinect_body* kbody = &frame->bodies[i];

    auto it = follower->bodies.find(kbody->identifier);
    if (it != follower->bodies.end()) {
      body* fb = &it->second;
      const AABB* aabb = &fb->kbody.body_box;
      Eigen::Vector2f meas;
      meas << aabb->top_left.x, aabb->top_left.y;
      kalman_predict(&fb->filter);
      kalman_update(&fb->filter, meas);

      fb->time_to_live = follower->body_time_to_live;
      fb->kbody = *kbody;

      const size_t history_limit = 512;
      if (fb->prev_positions.size() > history_limit) {
        fb->prev_positions.erase(
            fb->prev_positions.begin(),
            fb->prev_positions.begin() + history_limit / 2);
      }
      fb->prev_positions.push_back(kbody->body_box);

      if (fb->prev_kalmans.size() > history_limit) {
        fb->prev_kalmans.erase(fb->prev_kalmans.begin(),
                               fb->prev_kalmans.begin() + history_limit / 2);
      }
      fb->prev_kalmans.push_back({fb->filter.state(0), fb->filter.state(1)});

    } else {
      body b;
      b.time_to_live = follower->body_time_to_live;
      b.kbody = *kbody;
      const vec2 track_point = kbody->body_box.top_left;
      b.filter.state << track_point.x, track_point.y, 0, 0;
      b.filter.H << 1, 0, 0, 0, 0, 1, 0, 0;

      b.filter.P = Eigen::Matrix4f::Identity() * 1000.f;
      b.filter.R = Eigen::Matrix2f::Identity() * 1000.f;
      b.filter.F << 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1;

      b.filter.Q = Eigen::Matrix4f::Random() * 0.001f;

      follower->bodies[kbody->identifier] = b;
    }
  }
}

void follower_update(follower_ctx* follower, float dt) {
  auto& bodies = follower->bodies;

  for (auto it = bodies.begin(); it != bodies.end();) {
    body& b = it->second;
    b.time_to_live -= dt;

    if (b.time_to_live <= 0.0f) {
      it = bodies.erase(it);
    } else {
      it++;
    }
  }

  if (follower->has_target) {
    follower->target_ttl -= dt;
    if (follower->target_ttl <= 0.f) {
      follower->has_target = false;
    }
  }
}

depth_window calculate_range_map(const uint16_t* depth_data, uint32_t w,
                                 uint32_t h, float img_height_offset) {
  const uint32_t begin_row = static_cast<size_t>(img_height_offset * h);

  const uint16_t* depth_begin = depth_data + begin_row * w;

  depth_window intervals;

  const uint32_t rows = h - begin_row;
  const uint32_t interval_width = w / fl::DEPTH_RANGE_INTERVALS;

  uint32_t temp[fl::DEPTH_RANGE_INTERVALS];
  std::memset(&temp, 0, sizeof(temp));

  uint32_t interval_counted_values[fl::DEPTH_RANGE_INTERVALS] = {0};

  for (uint32_t r = 0; r < rows; r++) {
    for (uint32_t i = 0; i < fl::DEPTH_RANGE_INTERVALS; i++) {
      for (uint32_t c = 0; c < interval_width; c++) {
        uint32_t v = depth_begin[w * r + i * interval_width + c];
        if (v >= MIN_RELIABLE_DIST && v <= MAX_RELIABLE_DIST) {
          temp[i] += v;
          interval_counted_values[i] += 1;
        }
      }
    }
  }

  for (size_t i = 0; i < fl::DEPTH_RANGE_INTERVALS; i++) {
    const uint32_t v = temp[i];
    const uint32_t counted = interval_counted_values[i];
    if (counted > 0) {
      const uint32_t avg = v / interval_counted_values[i];
      intervals[i] = static_cast<uint16_t>(avg);
    } else {
      intervals[i] = 0;
    }
  }

  return intervals;
}

bool follower_begin_serial(follower_ctx* follower, const std::string& port,
                           uint32_t baud) {
  try {
    follower->serial.reset(new CallbackAsyncSerial(port, baud));
    return true;
  } catch (std::exception& e) {
    printf("failed to open serial: %s\n", e.what());
  }

  return false;
}

bool follower_has_serial(const follower_ctx* follower) {
  return follower->serial && follower->serial->isOpen();
}

void send_serial_message(follower_ctx* follower, const uint8_t* data,
                         size_t len) {
  if (follower->serial && follower->serial->isOpen()) {
    follower->serial->write((const char*)data, len);
  }
}

follower_ctx::~follower_ctx() {
  if (hog) hog_destroy(hog);
}

void follower_update_possible_position(follower_ctx* follower,
                                       const AABB* detections, size_t len) {
  follower->combined_hogs = aabb_combine_overlapping(detections, len);

  if (follower->combined_hogs.empty()) return;

  AABB possible_target;
  int num_nodes = 0;
  for (size_t i = 0; i < follower->combined_hogs.size(); i++) {
    const merged_aabb* node = &follower->combined_hogs[i];
    if (node->num_aabbs > num_nodes) {
      possible_target = node->aabb;
      num_nodes = node->num_aabbs;
    }
  }

  follower->has_target = true;
  follower->possible_target = possible_target;
  follower->target_ttl = follower->body_time_to_live;

  follower_update_camera_pos(follower, &possible_target);
}
