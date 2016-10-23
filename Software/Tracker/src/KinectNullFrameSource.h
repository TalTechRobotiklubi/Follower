#pragma once
#include "KinectFrameSource.h"

struct KinectNullFrameSource : KinectFrameSource {
  const KinectFrame* GetFrame() override { return nullptr; }
  void FillFrame(KinectFrame*) override {}
};
