#include "kinect_recording.h"
#include "kinect_frame.h"

namespace {

void push_u32(std::vector<uint8_t>& buffer, uint32_t val) {
	uint8_t* begin = (uint8_t*)&val;

	for (size_t i = 0; i < 4; i++) {
		const uint8_t* offset = begin + i;
		buffer.push_back(*offset);
	}
}

}

void recording_begin(recording_ctx* ctx, const char* file_name, uint32_t frames) {
	printf("will record %u frames to %s\n", frames, file_name);
	ctx->total_frames = frames;
	ctx->record_buffer.clear();
	ctx->record_buffer.reserve(1 << 30);
	ctx->file = fopen(file_name, "wb");

	push_u32(ctx->record_buffer, frames);
}

void recording_end(recording_ctx* ctx) {
	
	fwrite(ctx->record_buffer.data(), sizeof(uint8_t), ctx->record_buffer.size(), ctx->file);
	fclose(ctx->file);
	
	ctx->file = nullptr;
	printf("file size: %.2f mb\n", ctx->record_buffer.size() / 1000000.0);
}

void recording_add_frame(recording_ctx* ctx, const kinect_frame* frame, float frame_time) {

	if (ctx->cur_frame > ctx->total_frames) return;
	if (ctx->cur_frame == 1) ctx->initial_frame_time = frame_time;

  const uint32_t depth_length = (uint32_t)(frame->depth_length * sizeof(uint16_t));
	//const uint32_t rgb_length = frame->rgb_length;
	const uint32_t bodies_length = (uint32_t)(frame->num_bodies * sizeof(kinect_body));

	const uint32_t frame_length = depth_length + bodies_length;

	push_u32(ctx->record_buffer, frame_length);

	const float relative_frame_time = frame_time - ctx->initial_frame_time;
	printf("%.2f: record [%u/%u]\n", relative_frame_time, ctx->cur_frame, ctx->total_frames);
	const uint8_t* frame_time_begin = (const uint8_t*)&relative_frame_time;
	const uint8_t* frame_time_end = frame_time_begin + sizeof(relative_frame_time);

	ctx->record_buffer.insert(ctx->record_buffer.end(), frame_time_begin, frame_time_end);

	const uint8_t* depth_begin = (const uint8_t*)frame->depth_data;
	const uint8_t* depth_end = depth_begin + depth_length;
	ctx->record_buffer.insert(ctx->record_buffer.end(), depth_begin, depth_end);

	if (frame->num_bodies > 0) {
		const uint8_t* bodies_begin = (const uint8_t*)frame->bodies;
		const uint8_t* bodies_end = bodies_begin + bodies_length;
		ctx->record_buffer.insert(ctx->record_buffer.end(), bodies_begin, bodies_end);
	}

	ctx->cur_frame++;
}
