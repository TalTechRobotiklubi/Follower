#include "follower_ctx.h"
#include "fl_math.h"
#include <string.h>

void follower_update_camera_pos(follower_ctx* follower, const AABB* target) {
  const float target_x =
      (target->top_left.x + target->bot_right.x) * 0.5f / 512.f;
  const float target_y =
      (target->top_left.y + target->bot_right.y) * 0.5f / 424.f;

  follower->out_data.camera_degrees.x = fl_map_range(target_x, 0.f, 1.0f, -45.f, 45.f);
  follower->out_data.camera_degrees.y = fl_map_range(target_y, 0.f, 1.0f, 45.f, -45.f);
}

void follower_update(follower_ctx* follower, float dt) {
  if (follower->has_target) {
    follower->target.time_to_live -= dt;
    if (follower->target.time_to_live <= 0.f) {
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
  memset(&temp, 0, sizeof(temp));

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

follower_ctx::follower_ctx() {
  hod_ctx_init(&hod, 512, 424, 8, 8);
}

follower_ctx::~follower_ctx() {}

void body_target::update(const AABB* aabb, float dt) {
  (void)dt;

  if (num_positions < max_positions) {
    positions[num_positions++] = *aabb; 

    avg_position = *aabb;
  } else {
    for (int i = 0; i < max_positions - 1; i++) {
      positions[i] = positions[i + 1];
      positions[max_positions - 1] = *aabb;
    }

    AABB avg = { vec2{0.f, 0.f}, vec2{0.f, 0.f} };
    
    for (int i = 0; i < max_positions; i++) {
      const AABB* a = &positions[i];
      avg.top_left.x += a->top_left.x;
      avg.top_left.y += a->top_left.y;
      avg.bot_right.x += a->bot_right.x;
      avg.bot_right.y += a->bot_right.y;
    }

    const float mp = float(max_positions);
    avg.top_left.x /= mp;
    avg.top_left.y /= mp;
    avg.bot_right.x /= mp;
    avg.bot_right.y /= mp;

    avg_position = avg;
  }
}

void follower_update_candidates(follower_ctx* follower, float dt) {
  for (int i = 0; i < follower->hod.candidates_len; i++) {
    const hod_candidate* candidate = &follower->hod.candidates[i];
    if (candidate->weight <= 0.97f) continue;

    float x = float(candidate->depth_position.x);
    float y = float(candidate->depth_position.y);

    AABB position;
    position.top_left.x = x;
    position.top_left.y = y;
    position.bot_right.x = x + float(candidate->depth_position.width);
    position.bot_right.y = y + float(candidate->depth_position.height);

    follower->target.update(&position, dt);
    follower_update_camera_pos(follower, &position);

    if (!follower->has_target) {
      follower->has_target = true;
    }

    follower->target.time_to_live = 1.f;
  }
}
