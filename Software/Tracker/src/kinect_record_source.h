#ifndef KINECT_RECORD_SOURCE_H
#define KINECT_RECORD_SOURCE_H

#include <memory>
#include <stdint.h>
#include "kinect_frame_source.h"
#include "kinect_frame.h"

struct kinect_record_source : kinect_frame_source {

	kinect_record_source(std::unique_ptr<uint8_t[]> recording);

	const kinect_frame* get_frame(float dt) override;

	uint32_t frames = 0;
	uint32_t current_frame = 1;
  float total_time = 0.f;
	size_t offset = 0;
	std::unique_ptr<uint8_t[]> record_data;

	kinect_frame access_frame;
};

#endif
