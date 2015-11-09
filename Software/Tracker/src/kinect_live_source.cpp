#include "kinect_live_source.h"
#include "kinect_frame.h"
#include <Kinect.h>
#include <assert.h>
#include <float.h>
#include "fl_yuy2_convert.h"

namespace {

vec2 camerapos_to_depthpos(ICoordinateMapper* coord_mapper,
                           const CameraSpacePoint& pos) {
  DepthSpacePoint depthPoint = {0};
  coord_mapper->MapCameraPointToDepthSpace(pos, &depthPoint);
  return {depthPoint.X, depthPoint.Y};
}

template <class Interface>
void interface_release(Interface*& i) {
  if (i != nullptr) {
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

bool read_infrared_data(kinect_live_source* source) {
  bool got_data = false;

  IInfraredFrame* frame = nullptr;

  HRESULT hr = source->infrared_reader->AcquireLatestFrame(&frame);

  if (SUCCEEDED(hr)) {
    uint32_t buffer_size = 0;
    uint16_t* buffer = nullptr;
    hr = frame->AccessUnderlyingBuffer(&buffer_size, &buffer);

    if (SUCCEEDED(hr)) {
      source->infrared_buffer.clear();

      const uint16_t* begin = (const uint16_t*)buffer;
      const uint16_t* end = (const uint16_t*)buffer + buffer_size;

      source->infrared_buffer.insert(source->infrared_buffer.end(), begin, end);

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
    IFrameDescription* frame_desc;
    frame->get_FrameDescription(&frame_desc);

    ColorImageFormat img_format;
    frame->get_RawColorImageFormat(&img_format);

    assert(img_format == ColorImageFormat_Yuy2);

    int width, height;
    unsigned int bpp;
    frame_desc->get_Width(&width);
    frame_desc->get_Height(&height);
    frame_desc->get_BytesPerPixel(&bpp);

    UINT capacity;
    BYTE* yuy2_buffer;
    frame->AccessRawUnderlyingBuffer(&capacity, &yuy2_buffer);
    source->kinect_rgba_buf =
        fl_yuy2_to_rgba(source->yuy2_converter, yuy2_buffer);
  }

  interface_release(frame);

  return got_data;
}

size_t read_body_data(kinect_live_source* source) {
  size_t num_bodies_found = 0;

  IBodyFrame* body_frame = nullptr;

  auto res = source->body_reader->AcquireLatestFrame(&body_frame);

  if (SUCCEEDED(res)) {
    IBody* bodies[BODY_COUNT] = {0};

    res = body_frame->GetAndRefreshBodyData(BODY_COUNT, bodies);

    if (SUCCEEDED(res)) {
      for (IBody* body : bodies) {
        BOOLEAN tracked = false;
        body->get_IsTracked(&tracked);
        if (tracked) {
          Joint joints[JointType_Count];
          if (SUCCEEDED(body->GetJoints(JointType_Count, joints))) {
            AABB bbox = {{FLT_MAX, FLT_MAX}, {-FLT_MAX, -FLT_MAX}};
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
                vec2 depth_pos =
                    camerapos_to_depthpos(source->coord_mapper, pos);
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

  yuy2_converter = fl_yuy2_convert_create(1920, 1080, 320, 240);

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
      kinect_rgba_buf_len = 320 * 240 * 4;
    }

    IInfraredFrameSource* infrared_source = nullptr;
    hr = kinect->get_InfraredFrameSource(&infrared_source);

    if (SUCCEEDED(hr)) {
      hr = infrared_source->OpenReader(&infrared_reader);
    }

    kinect->get_CoordinateMapper(&coord_mapper);
    IBodyFrameSource* body_source = nullptr;

    kinect->get_BodyFrameSource(&body_source);
    body_source->OpenReader(&body_reader);

    assert(body_reader != nullptr);

    interface_release(body_source);
    interface_release(depth_source);
    interface_release(color_source);
    interface_release(infrared_source);
  }
}

kinect_live_source::~kinect_live_source() {
  if (kinect) {
    kinect->Close();
    interface_release(kinect);
  }

  fl_yuy2_convert_destroy(yuy2_converter);
}

const kinect_frame* kinect_live_source::get_frame(float) {
  if (read_depth_data(this)) {
    read_infrared_data(this);
    read_rgb_data(this);

    body_buffer.clear();
    read_body_data(this);

    current_frame.depth_data = depth_buffer.data();
    current_frame.depth_length = depth_buffer.size();
    current_frame.infrared_data = infrared_buffer.data();
    current_frame.infrared_length = infrared_buffer.size();
    current_frame.bodies = body_buffer.data();
    current_frame.num_bodies = body_buffer.size();
    current_frame.rgba_width = 320;
    current_frame.rgba_height = 240;
    current_frame.rgba_data = kinect_rgba_buf;
    current_frame.rgba_length = kinect_rgba_buf_len;

    return &current_frame;
  }

  return nullptr;
}
