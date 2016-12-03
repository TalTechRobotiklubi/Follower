#pragma once
#include <thread>
#include "BlockDiff.h"
#include "CoreObj.h"
#include "Image.h"
#include "KinectFrame.h"
#include "ScriptLoader.h"
#include "classifier/Classifier.h"
#include "comm/comm_input.h"
#include "comm/comm_output.h"
#include "comm/serial_comm.h"
#include "proto/flatbuffers/flatbuffers.h"

struct core {
  double timestamp = 0.0;
  float dtMilli = 0.0;
  bool sendVideo = true;
  bool sendDebugData = false;
  struct SqliteFrameWriter* writer = nullptr;
  World* world = nullptr;
  TrackingState tracking;
  ControlState state;
  CommInput in_data;
  CommOutput out_data;
  SerialComm serial;
  std::thread kinect_frame_thread;
  int frameNum = 0;
  KinectFrame kinectFrame;
  struct KinectFrameSource* frameSource = nullptr;
  struct UdpHost* udp = nullptr;
  ScriptLoader scripts;

  RgbaImage rgba_depth;
  RgbaImage prev_rgba_depth;
  ActiveMap rgba_depth_diff;
  struct Encoder* encoder;
  IoVec encoded_depth;

  struct fhd_context* fhd = nullptr;

  flatbuffers::FlatBufferBuilder builder;
  std::vector<std::unique_ptr<Classifier>> classifiers;

  core();
  ~core();
};

void core_start(core* c);
