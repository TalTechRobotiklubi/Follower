#include "follower_ctx.h"
#include "fl_math.h"

void follower_update_camera_pos(follower_ctx* follower, const AABB* target) {
  const float target_x =
      (target->top_left.x + target->bot_right.x) * 0.5f / 512.f;
  const float target_y =
      (target->top_left.y + target->bot_right.y) * 0.5f / 424.f;

  follower->out_data.camera_degrees.x = fl_map_range(target_x, 0.f, 1.0f, -45.f, 45.f);
  follower->out_data.camera_degrees.y = fl_map_range(target_y, 0.f, 1.0f, 45.f, -45.f);
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

void follower_update(follower_ctx* follower, float dt) {
  if (follower->has_target) {
    follower->possible_target.time_to_live -= dt;
    if (follower->possible_target.time_to_live <= 0.f) {
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

follower_ctx::follower_ctx() {}

follower_ctx::~follower_ctx() {}

void follower_update_possible_position(follower_ctx* follower,
                                       const AABB* detections, size_t len) {
  follower->combined_hogs = aabb_combine_overlapping(detections, len);

  if (follower->combined_hogs.empty()) return;

  AABB possible_target_pos;
  int num_nodes = 0;
  for (size_t i = 0; i < follower->combined_hogs.size(); i++) {
    const merged_aabb* node = &follower->combined_hogs[i];
    if (node->num_aabbs > num_nodes) {
      possible_target_pos = node->aabb;
      num_nodes = node->num_aabbs;
    }
  }

  follower->has_target = true;
  follower->possible_target.location = possible_target_pos;
  follower->possible_target.time_to_live = follower->body_time_to_live;
  follower->possible_target.update_kalman(&possible_target_pos);

  follower_update_camera_pos(follower, &follower->possible_target.location);
}

body_target::body_target()
  : kf(6, 4, 0)
  , z_t(4, 1, CV_32F)
  , x_t(6, 1, CV_32F)
  , w_k(6, 1, CV_32F) {

  w_k << 0.05f, 0.05f, 0.05f, 0.05f, 0.1f, 0.1f;

  z_t.setTo(cv::Scalar(0));

  x_t << 256.f, 212.f, 256.f, 212.f, 0.f, 0.f;

  cv::Mat_<float> transition_matrix(6, 6);
  transition_matrix <<
    1.f, 0.f, 0.f, 0.f, 1.f, 0.f,
    0.f, 1.f, 0.f, 0.f, 0.f, 1.f,
    0.f, 0.f, 1.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f, 0.f, 1.f,
    0.f, 0.f, 0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 0.f, 0.f, 1.f;

  kf.transitionMatrix = transition_matrix;

  cv::Mat_<float> measurement_matrix(4, 6);
  measurement_matrix <<
    1.f, 0.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f, 0.f, 0.f,
    0.f, 0.f, 0.f, 1.f, 0.f, 0.f;

  kf.measurementMatrix = measurement_matrix;

  cv::Mat_<float> process_noise_cov(6, 6);
  process_noise_cov <<
    0.25f, 0.f, 0.f, 0.f, 0.5f, 0.f,
    0.f, 0.25f, 0.f, 0.f, 0.f, 0.5f,
    0.f, 0.f, 0.25f, 0.f, 0.5f, 0.f,
    0.f, 0.f, 0.f, 0.25f, 0.f, 0.5f,
    0.5f, 0.f, 0.5f, 0.f, 1.f, 0.f,
    0.f, 0.5f, 0.f, 0.5, 0.f, 1.f;

  kf.processNoiseCov = process_noise_cov * 0.001f;

  setIdentity(kf.measurementNoiseCov, cv::Scalar::all(500.f));
  setIdentity(kf.errorCovPost, cv::Scalar::all(1e+5));
  x_t.copyTo(kf.statePost);
}

void body_target::update_kalman(const AABB* aabb) {
  const vec2 tl = aabb->top_left;
  const vec2 br = aabb->bot_right;

  cv::Mat predicted = kf.predict();
  kf_prediction.top_left.x = predicted.at<float>(0);  
  kf_prediction.top_left.y = predicted.at<float>(1);  
  kf_prediction.bot_right.x = predicted.at<float>(2);  
  kf_prediction.bot_right.y = predicted.at<float>(3);  

  z_t << tl.x, tl.y, br.x, br.y;

  kf.correct(z_t);
}
