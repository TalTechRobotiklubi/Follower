#ifndef KINECT_LIVE_SOURCE_H
#define KINECT_LIVE_SOURCE_H

#include "kinect_frame_source.h"
#include "kinect_frame.h"
#include <vector>
#include <cstdint>

struct IKinectSensor;
struct IDepthFrameReader;
struct IBodyFrameReader;
struct ICoordinateMapper;
struct IColorFrameReader;

struct kinect_live_source : kinect_frame_source {

	kinect_live_source();
	~kinect_live_source();

	const kinect_frame* get_frame(float dt) override;

	IKinectSensor* kinect = nullptr;
	IDepthFrameReader* depth_reader = nullptr;
  IColorFrameReader* color_reader = nullptr;
	IBodyFrameReader* body_reader = nullptr;
	ICoordinateMapper* coord_mapper = nullptr;

	std::vector<kinect_body> body_buffer;
	std::vector<uint16_t> depth_buffer;
  
	uint8_t* kinect_rgba_buf = nullptr;
	size_t kinect_rgba_buf_len = 0;

	uint8_t* rgba_buf = nullptr;
	size_t rgba_buf_len;
	int rgb_width = 320;
	int rgb_height = 240;

	kinect_frame current_frame;
};

#endif
