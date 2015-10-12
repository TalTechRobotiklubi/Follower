#ifndef KINECT_RECORDING_H
#define KINECT_RECORDING_H

#include <stdio.h>
#include <cstdint>
#include <vector>

struct kinect_frame;

struct recording_ctx {
	FILE* file = nullptr;
	uint32_t total_frames = 0;
	uint32_t cur_frame = 1;
	float initial_frame_time = 0.f;

	std::vector<uint8_t> record_buffer;

	bool recording() const { return file != nullptr; }
};

void recording_begin(recording_ctx* ctx, const char* file_name, uint32_t frames);
void recording_end(recording_ctx* ctx);
void recording_add_frame(recording_ctx* ctx, const kinect_frame* frame, float frame_time);

#endif
