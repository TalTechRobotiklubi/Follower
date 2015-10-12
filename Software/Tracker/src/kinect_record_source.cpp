#include "kinect_record_source.h"
#include <cstring>

struct frame_header {
	uint32_t frame_size;
	float frame_time;
};

kinect_record_source::kinect_record_source(std::unique_ptr<uint8_t[]> recording)
	: record_data(std::move(recording)) {

	const uint8_t* begin = record_data.get();
	std::memcpy(&frames, begin, 4);

	offset += 4;

	printf("record frames: %u\n", frames);
}

const kinect_frame* kinect_record_source::get_frame(float dt) {

  total_time += dt;

	if (current_frame > frames) {
		offset = 4;
		current_frame = 1;
    total_time = 0.f;
	}

	uint8_t* frame_begin = record_data.get() + offset;

	frame_header header;
	std::memcpy(&header, frame_begin, sizeof(header));

  if (total_time < header.frame_time) {
    return nullptr;
  }

	offset += sizeof(header);

	const size_t depth_frame_length = 512 * 424;
	const size_t depth_frame_bytes = depth_frame_length * sizeof(uint16_t);

	std::memset(&access_frame, 0, sizeof(access_frame));
	uint8_t* depth_begin = record_data.get() + offset;
	access_frame.depth_data = (uint16_t*)depth_begin;
	access_frame.depth_length = depth_frame_length;

	offset += header.frame_size;

	const size_t body_bytes = header.frame_size - depth_frame_bytes;
	if (body_bytes > 0) {
		const size_t num_bodies = body_bytes / sizeof(kinect_body);
		access_frame.bodies = (kinect_body*)(frame_begin + sizeof(header) + depth_frame_bytes);
		access_frame.num_bodies = num_bodies;
	}

	current_frame++;

	return &access_frame;
}
