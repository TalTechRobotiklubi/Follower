#pragma once
#include <stdint.h>

struct KinectFrame {
  int depthLength = 0;
  uint16_t* depthData = nullptr;

  int rgbaWidth = 0;
  int rgbaHeight = 0;
  int rgbaLength = 0;
  uint8_t* rgbaData = nullptr;
};

void KinectFrameInit(KinectFrame* frame, int width, int height);
void CopyKinectFrame(const KinectFrame* src, KinectFrame* dst);
