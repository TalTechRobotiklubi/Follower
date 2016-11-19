#pragma once
#include <thread>
#include "comm/serial_comm.h"
#include "comm/comm_input.h"
#include "comm/comm_output.h"
#include "KinectFrame.h"
#include "image.h"
#include "BlockDiff.h"
#include "proto/flatbuffers/flatbuffers.h"
#include "ScriptLoader.h"
#include "CoreObj.h"
#include "classifier/Classifier.h"
#include "ColorArea.h"

struct core {
  double timestamp = 0.0;
  float dtMilli = 0.0;
  bool sendVideo = true;
  struct fl_sqlite_writer* writer = nullptr;
  World world;
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

  rgba_image rgba_depth;
  rgba_image prev_rgba_depth;
  ActiveMap rgba_depth_diff;
  struct Encoder* encoder;
  IoVec encoded_depth;

  struct fhd_context* fhd = nullptr;

  flatbuffers::FlatBufferBuilder builder;
  std::vector<std::unique_ptr<Classifier>> classifiers;
	std::vector<ColorArea> candidateColors;

	core();
  ~core();
};

void core_start(core* c);
