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
#include "ScriptLoader.h"
#include "CoreObj.h"

struct core {
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
  ScriptLoader scripts;

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
