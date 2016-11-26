#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>
#include <vector>
#include "KinectFrame.h"
#include "KinectFrameSource.h"

struct IKinectSensor;
struct IDepthFrameReader;
struct IColorFrameReader;
struct fl_yuy2_convert;

struct KinectLiveFrameSource : KinectFrameSource {
  KinectLiveFrameSource();
  ~KinectLiveFrameSource();

  const KinectFrame* GetFrame() override;
  void FillFrame(KinectFrame* dst) override;
  int FrameNumber() const override;

  IKinectSensor* kinect = nullptr;
  IDepthFrameReader* depthReader = nullptr;
  IColorFrameReader* colorReader = nullptr;

  fl_yuy2_convert* yuy2Converter = nullptr;
  std::vector<uint16_t> depthBuffer;

  uint8_t* kinectRgbaBuf = nullptr;
  size_t kinectRgbaBufLen = 0;
  std::atomic<int> frameNumber;

  KinectFrame currentFrame;
  std::mutex frameLock;
};
