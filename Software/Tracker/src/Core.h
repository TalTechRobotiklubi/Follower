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

struct Core {
  double timestamp = 0.0;
  double dtMilli = 0.0;
  bool sendVideo = true;
  bool sendDebugData = false;
  struct SqliteFrameWriter* writer = nullptr;
  World* world = nullptr;
  TrackingState tracking;
  ControlState state;
  CommInput inData;
  CommOutput outData;
  SerialComm serial;
  std::thread kinectFrameThread;
  int frameNum = 0;
  KinectFrame kinectFrame;
  struct KinectFrameSource* frameSource = nullptr;
  struct UdpHost* udp = nullptr;
  ScriptLoader scripts;

  RgbaImage rgbaDepth;
  RgbaImage prevRgbaDepth;
  ActiveMap rgbaDepthDiff;
  struct Encoder* encoder;
  IoVec encodedDepth;

  struct fhd_context* fhd = nullptr;

  flatbuffers::FlatBufferBuilder builder;
  std::vector<std::unique_ptr<Classifier>> classifiers;

  Core();
  ~Core();
};

void CoreStart(Core* c);
