#include "follower_ctx.h"
#include "kinect_frame_source.h"
#include "interp/cubic_hermite.h"
#include "hog_detect.h"

void add_spline(std::vector<vec2>* spline, vec2 a, vec2 b, vec2 c, vec2 d) {

  float t = 0.f;
  for (size_t i = 0; i < 10; i++) {
    float x = cubic_hermite(a.x, b.x, c.x, d.x, t, 0.f, 0); 
    float y = cubic_hermite(a.y, b.y, c.y, d.y, t, 0.f, 0); 
    t += 0.1f;

    spline->push_back({x, y});
  }
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
        fb->prev_positions.erase(fb->prev_positions.begin(), fb->prev_positions.begin() + history_limit / 2);
      }
      fb->prev_positions.push_back(kbody->body_box);

      if (fb->prev_kalmans.size() > history_limit) {
        fb->prev_kalmans.erase(fb->prev_kalmans.begin(), fb->prev_kalmans.begin() + history_limit / 2);
      }
      fb->prev_kalmans.push_back({ fb->filter.state(0), fb->filter.state(1) });
      
    } else {
      body b;
      b.time_to_live = follower->body_time_to_live;
      b.kbody = *kbody;
      const vec2 track_point = kbody->body_box.top_left;
      b.filter.state << track_point.x, track_point.y, 0, 0;
      b.filter.H <<
        1, 0, 0, 0,
        0, 1, 0, 0;

      b.filter.P = Eigen::Matrix4f::Identity() * 1000.f;
      b.filter.R = Eigen::Matrix2f::Identity() * 1000.f;
      b.filter.F <<
        1, 0, 1, 0,
        0, 1, 0, 1,
        0, 0, 1, 0,
        0, 0, 0, 1;

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

}

depth_window calculate_range_map(const uint16_t* depth_data,
                                 uint32_t w,
                                 uint32_t h,
                                 float img_height_offset) {
	const uint32_t begin_row = static_cast<size_t>(img_height_offset * h);

	const uint16_t* depth_begin = depth_data + begin_row * w;

	depth_window intervals;

	const uint32_t rows = h - begin_row;
	const uint32_t interval_width = w / range_intervals;

	uint32_t temp[range_intervals];
	std::memset(&temp, 0, sizeof(temp));

	uint32_t interval_counted_values[range_intervals] = { 0 };

	for (uint32_t r = 0; r < rows; r++) {
		for (uint32_t i = 0; i < range_intervals; i++) {
			for (uint32_t c = 0; c < interval_width; c++) {
				uint32_t v = depth_begin[w * r + i * interval_width + c];
				if (v >= MIN_RELIABLE_DIST && v <= MAX_RELIABLE_DIST) {
					temp[i] += v;
					interval_counted_values[i] += 1;
				}
			}
		}
	}

	for (size_t i = 0; i < range_intervals; i++) {
		const uint32_t v = temp[i];
		const uint32_t counted = interval_counted_values[i];
		if (counted > 0) {
			const uint32_t avg = v / interval_counted_values[i];
			intervals[i] = static_cast<uint16_t>(avg);
		}
		else {
			intervals[i] = 0;
		}
	}

	return intervals;
}

bool follower_begin_serial(follower_ctx* follower, const std::string& port, uint32_t baud) {
  try {
    follower->serial.reset(new CallbackAsyncSerial(port, baud));
    return true;
  }
  catch (std::exception& e) {
    printf("failed to open serial: %s\n", e.what());
  }

  return false;
}

bool follower_has_serial(const follower_ctx* follower) {
  return follower->serial && follower->serial->isOpen();
}

void send_serial_message(follower_ctx* follower, const uint8_t* data, size_t len) {
	if (follower->serial && follower->serial->isOpen()) {
		follower->serial->write((const char*)data, len);
		for (size_t i = 0; i < len; i++) {
			printf("%x ", data[i]);
		}
		printf("\nsent %lu bytes\n", len);
	}
}

follower_ctx::~follower_ctx() {
  if (hog) hog_destroy(hog);
}

