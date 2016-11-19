#pragma once

struct KinectFrame;

struct KinectFrameSource {
  virtual const KinectFrame* GetFrame() = 0;
  virtual void FillFrame(KinectFrame* dst) = 0;
  virtual int FrameNumber() const { return 0; }
  virtual ~KinectFrameSource() {}
};
