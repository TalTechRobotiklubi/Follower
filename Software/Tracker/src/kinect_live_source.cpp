#include "kinect_live_source.h"
#include "kinect_frame.h"
#include <Kinect.h>
#include <assert.h>
#include <float.h>
#include "stb_image_resize.h"
#include <chrono>

namespace {

	vec2 camerapos_to_depthpos(ICoordinateMapper* coord_mapper, const CameraSpacePoint& pos) {
		DepthSpacePoint depthPoint = { 0 };
		coord_mapper->MapCameraPointToDepthSpace(pos, &depthPoint);
		return{
			depthPoint.X,
			depthPoint.Y
		};
	}

	template<class Interface>
	void interface_release(Interface*& i)
	{
		if (i != nullptr)
		{
			i->Release();
			i = nullptr;
		}
	}

	bool read_depth_data(kinect_live_source* source) {

		bool got_data = false;

		IDepthFrame* frame = nullptr;

		HRESULT hr = source->depth_reader->AcquireLatestFrame(&frame);

		if (SUCCEEDED(hr)) {
			uint32_t buffer_size = 0;
			uint16_t* buffer = nullptr;
			hr = frame->AccessUnderlyingBuffer(&buffer_size, &buffer);

			if (SUCCEEDED(hr)) {
				source->depth_buffer.clear();

				const uint16_t* begin = (const uint16_t*)buffer;
				const uint16_t* end = (const uint16_t*)buffer + buffer_size;

				source->depth_buffer.insert(source->depth_buffer.end(), begin, end);

				got_data = true;
			}

		}

		interface_release(frame);

		return got_data;
	}

  bool read_rgb_data(kinect_live_source* source) {

    bool got_data = false;

    IColorFrame* frame = nullptr;

    HRESULT hr = source->color_reader->AcquireLatestFrame(&frame);

    if (SUCCEEDED(hr)) {
			int width, height;
			unsigned int bpp;

			ColorImageFormat requested_format = ColorImageFormat_Rgba;

			IFrameDescription* req_frame_desc;
			frame->CreateFrameDescription(requested_format, &req_frame_desc);
			req_frame_desc->get_Width(&width);
			req_frame_desc->get_Height(&height);
			req_frame_desc->get_BytesPerPixel(&bpp);

			assert(requested_format == ColorImageFormat_Rgba);
			assert(source->rgb_length == width * height * bpp);
			frame->CopyConvertedFrameDataToArray(source->kinect_rgba_buf_len, source->kinect_rgba_buf, requested_format);
			
			stbir_resize_uint8(source->kinect_rgba_buf,
				width,
				height,
				width * bpp,
				source->rgba_buf, source->rgb_width,
				source->rgb_height,
				source->rgb_width * 4,
				4);
			
    }

    interface_release(frame);

    return got_data;
  }

	size_t read_body_data(kinect_live_source* source) {

		size_t num_bodies_found = 0;

		IBodyFrame* body_frame = nullptr;

		auto res = source->body_reader->AcquireLatestFrame(&body_frame);

		if (SUCCEEDED(res)) {
			IBody* bodies[BODY_COUNT] = { 0 };

			res = body_frame->GetAndRefreshBodyData(BODY_COUNT, bodies);

			if (SUCCEEDED(res)) {
				for (IBody* body : bodies) {
					BOOLEAN tracked = false;
					body->get_IsTracked(&tracked);
					if (tracked) {
						Joint joints[JointType_Count];
						if (SUCCEEDED(body->GetJoints(JointType_Count, joints))) {

							AABB bbox = {
								{ FLT_MAX, FLT_MAX },
								{ -FLT_MAX, -FLT_MAX }
							};
							size_t active_joints = 0;
							for (Joint& joint : joints) {
								if (joint.TrackingState == TrackingState_Tracked &&
										joint.JointType != JointType_HandTipLeft &&
										joint.JointType != JointType_HandLeft &&
										joint.JointType != JointType_ThumbLeft &&
										joint.JointType != JointType_WristLeft &&
										joint.JointType != JointType_HandTipRight &&
										joint.JointType != JointType_HandRight &&
										joint.JointType != JointType_ThumbRight &&
										joint.JointType != JointType_WristRight) {
									auto pos = joint.Position;
									vec2 depth_pos = camerapos_to_depthpos(source->coord_mapper, pos);
									aabb_expand(&bbox, depth_pos);
									active_joints++;
								}
							}

							if (active_joints > 0) {
								kinect_body b;
								body->get_TrackingId(&b.identifier);
								b.body_box = bbox;
								source->body_buffer.push_back(b);
								num_bodies_found++;
							}
						}
					}
				}
			}

			for (IBody* body : bodies) {
				interface_release(body);
			}
		}

		interface_release(body_frame);

		return num_bodies_found;
	}


}

kinect_live_source::kinect_live_source() {
	HRESULT hr = GetDefaultKinectSensor(&kinect);
	if (FAILED(hr)) {
		fprintf(stderr, "Failed to get the kinect sensor\n");
	}

	if (kinect) {
		hr = kinect->Open();
		if (FAILED(hr)) {
			fprintf(stderr, "Failed to open kinect sensor\n");
		}

		IDepthFrameSource* depth_source = nullptr;
		hr = kinect->get_DepthFrameSource(&depth_source);

		if (SUCCEEDED(hr)) {
			hr = depth_source->OpenReader(&depth_reader);
		}

    IColorFrameSource* color_source = nullptr;
    hr = kinect->get_ColorFrameSource(&color_source);

    if (SUCCEEDED(hr)) {
      hr = color_source->OpenReader(&color_reader);
			kinect_rgba_buf_len = 1920 * 1080 * 4;
			kinect_rgba_buf = (uint8_t*)malloc(kinect_rgba_buf_len);

			rgba_buf_len = rgb_width * rgb_height * 4;
			rgba_buf = (uint8_t*)malloc(rgba_buf_len);
    }

		kinect->get_CoordinateMapper(&coord_mapper);
		IBodyFrameSource* body_source = nullptr;

		kinect->get_BodyFrameSource(&body_source);
		body_source->OpenReader(&body_reader);

		assert(body_reader != nullptr);

		interface_release(body_source);
		interface_release(depth_source);
		interface_release(color_source);
	}
}

kinect_live_source::~kinect_live_source() {
	if (kinect) {
		kinect->Close();
		interface_release(kinect);
	}
}

const kinect_frame* kinect_live_source::get_frame(float) {

  

	if (read_depth_data(this)) {
		read_rgb_data(this);

		body_buffer.clear();
		read_body_data(this);

		current_frame.depth_data = depth_buffer.data();
		current_frame.depth_length = depth_buffer.size();
		current_frame.bodies = body_buffer.data();
		current_frame.num_bodies = body_buffer.size();
		current_frame.rgb_width = rgb_width;
		current_frame.rgb_height = rgb_height;
		current_frame.rgb_data = rgba_buf;
		current_frame.rgb_length = rgba_buf_len;

		return &current_frame;
	}

	return nullptr;
}
