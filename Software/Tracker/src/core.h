#pragma once
#include <atomic>
#include <thread>
#include "comm/serial_comm.h"
#include "comm/comm_input.h"
#include "comm/comm_output.h"
#include "kinect_frame.h"
#include "image.h"
#include "BlockDiff.h"
#include "proto/flatbuffers/flatbuffers.h"
#include "Target.h"

enum CoreState {
  kCheck = 0,
  kFind = 1
};

const int kMaxDetections = 8;

struct Detection {
  vec2 kinectPosition;
  vec3 metricPosition;
  float weight;
};

struct World {
  double timestamp = 0.0;
  int32_t numDetections;
  Detection detections[kMaxDetections];
};

struct TrackingState {
  std::vector<Target> targets;
};

struct ControlState {
  vec2 camera = {0.f, 0.f};
};

struct core {
  CoreState coreState = kCheck;
  double timestamp = 0.0;
  World world;
  TrackingState tracking;
  ControlState state;
  CommInput in_data;
  CommOutput out_data;
  SerialComm serial;
  std::thread kinect_frame_thread;
  std::atomic_bool running;
  kinect_frame current_frame;
  struct kinect_frame_source* frame_source = nullptr;
  struct UdpHost* udp = nullptr;

  rgba_image rgba_depth;
  rgba_image prev_rgba_depth;
  ActiveMap rgba_depth_diff;
  struct Encoder* encoder;
  IoVec encoded_depth;

  struct fhd_context* fhd = nullptr;

  flatbuffers::FlatBufferBuilder builder;

  core() { running = ATOMIC_VAR_INIT(true); }
  ~core();
};

void core_start(core* c);
