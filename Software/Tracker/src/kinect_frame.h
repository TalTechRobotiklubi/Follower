#ifndef KINECT_FRAME_H
#define KINECT_FRAME_H

#include <stdint.h>
#include "AABB.h"

struct kinect_body {
	AABB body_box;
	uint64_t identifier;
};

struct kinect_frame {
	uint16_t* depth_data;
	size_t depth_length;

	int rgb_width;
	int rgb_height;
  uint8_t* rgb_data = nullptr;
  size_t rgb_length;

	kinect_body* bodies;
	size_t num_bodies;
};

#endif
