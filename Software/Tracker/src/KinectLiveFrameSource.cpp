#include "KinectLiveFrameSource.h"
#include <Kinect.h>
#include <assert.h>
#include <float.h>
#include "Clock.h"
#include "Constants.h"
#include "Yuy2Convert.h"

namespace {

template <class Interface>
void InterfaceRelease(Interface*& i) {
  if (i != nullptr) {
    i->Release();
    i = nullptr;
  }
}

bool ReadDepthData(KinectLiveFrameSource* source) {
  bool gotData = false;

  IDepthFrame* frame = nullptr;

  HRESULT hr = source->depthReader->AcquireLatestFrame(&frame);

  if (SUCCEEDED(hr)) {
    uint32_t bufferSize = 0;
    uint16_t* buffer = nullptr;
    hr = frame->AccessUnderlyingBuffer(&bufferSize, &buffer);

    if (SUCCEEDED(hr)) {
      const uint16_t* begin = (const uint16_t*)buffer;
      const uint16_t* end = (const uint16_t*)buffer + bufferSize;

      std::unique_lock<std::mutex> lock(source->frameLock);
      source->depthBuffer.clear();
      source->depthBuffer.insert(source->depthBuffer.end(), begin, end);
      source->currentFrame.depthData = source->depthBuffer.data();
      source->currentFrame.depthLength = int(source->depthBuffer.size());
      lock.unlock();

      gotData = true;
    }
  }

  InterfaceRelease(frame);

  return gotData;
}

bool ReadRgbData(KinectLiveFrameSource* source) {
  bool gotData = false;

  IColorFrame* frame = nullptr;

  HRESULT hr = source->colorReader->AcquireLatestFrame(&frame);
  if (SUCCEEDED(hr)) {
    IFrameDescription* frameDesc;
    frame->get_FrameDescription(&frameDesc);

    ColorImageFormat imgFormat;
    frame->get_RawColorImageFormat(&imgFormat);

    assert(imgFormat == ColorImageFormat_Yuy2);

    int width, height;
    unsigned int bpp;
    frameDesc->get_Width(&width);
    frameDesc->get_Height(&height);
    frameDesc->get_BytesPerPixel(&bpp);

    UINT capacity;
    BYTE* yuy2Buffer;
    frame->AccessRawUnderlyingBuffer(&capacity, &yuy2Buffer);

    std::unique_lock<std::mutex> lock(source->frameLock);
    source->kinectRgbaBuf = Yuy2ToRgba(source->yuy2Converter, yuy2Buffer);
    source->currentFrame.rgbaWidth = kDepthWidth;
    source->currentFrame.rgbaHeight = kDeptHeight;
    source->currentFrame.rgbaData = source->kinectRgbaBuf;
    source->currentFrame.rgbaLength = int(source->kinectRgbaBufLen);
    lock.unlock();
  }

  InterfaceRelease(frame);
  return gotData;
}
}

KinectLiveFrameSource::KinectLiveFrameSource() : frameNumber(0) {
  HRESULT hr = GetDefaultKinectSensor(&kinect);
  if (FAILED(hr)) {
    fprintf(stderr, "Failed to get the kinect sensor\n");
  }

  yuy2Converter = Yuy2ConvertCreate(1920, 1080, kDepthWidth, kDeptHeight);

  if (kinect) {
    hr = kinect->Open();
    if (FAILED(hr)) {
      fprintf(stderr, "Failed to open kinect sensor\n");
    }

    IDepthFrameSource* depthSource = nullptr;
    hr = kinect->get_DepthFrameSource(&depthSource);

    if (SUCCEEDED(hr)) {
      hr = depthSource->OpenReader(&depthReader);
    }

    IColorFrameSource* colorSource = nullptr;
    hr = kinect->get_ColorFrameSource(&colorSource);

    if (SUCCEEDED(hr)) {
      hr = colorSource->OpenReader(&colorReader);
      kinectRgbaBufLen = kDepthWidth * kDeptHeight * 4;
    }

    InterfaceRelease(depthSource);
    InterfaceRelease(colorSource);
  }
}

KinectLiveFrameSource::~KinectLiveFrameSource() {
  if (kinect) {
    kinect->Close();
    InterfaceRelease(kinect);
  }

  Yuy2ConvertDestroy(yuy2Converter);
}

const KinectFrame* KinectLiveFrameSource::GetFrame() {
  if (ReadDepthData(this)) {
    ReadRgbData(this);
    frameNumber += 1;

    return &currentFrame;
  }

  return nullptr;
}

void KinectLiveFrameSource::FillFrame(KinectFrame* dst) {
  std::lock_guard<std::mutex> lock(frameLock);
  CopyKinectFrame(&currentFrame, dst);
}

int KinectLiveFrameSource::FrameNumber() const { return frameNumber.load(); }
