#include <Kinect.h>
#include <assert.h>
#include <float.h>
#include "Clock.h"
#include "KinectLiveFrameSource.h"
#include "fl_yuy2_convert.h"
#include "Constants.h"

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
      source->depthBuffer.clear();

      const uint16_t* begin = (const uint16_t*)buffer;
      const uint16_t* end = (const uint16_t*)buffer + bufferSize;

      source->depthBuffer.insert(source->depthBuffer.end(), begin, end);

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
    source->kinectRgbaBuf = fl_yuy2_to_rgba(source->yuy2Converter, yuy2Buffer);
  }

  InterfaceRelease(frame);
  return gotData;
}
}

KinectLiveFrameSource::KinectLiveFrameSource()
  : frameNumber(0) {
  HRESULT hr = GetDefaultKinectSensor(&kinect);
  if (FAILED(hr)) {
    fprintf(stderr, "Failed to get the kinect sensor\n");
  }

  yuy2Converter = fl_yuy2_convert_create(1920, 1080, kDepthWidth, kDeptHeight);

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

  fl_yuy2_convert_destroy(yuy2Converter);
}

const KinectFrame* KinectLiveFrameSource::GetFrame() {
  if (ReadDepthData(this)) {
    ReadRgbData(this);

    currentFrame.depthData = depthBuffer.data();
    currentFrame.depthLength = int(depthBuffer.size());
    currentFrame.rgbaWidth = kDepthWidth;
    currentFrame.rgbaHeight = kDeptHeight;
    currentFrame.rgbaData = kinectRgbaBuf;
    currentFrame.rgbaLength = int(kinectRgbaBufLen);
    frameNumber += 1;

    return &currentFrame;
  }

  return nullptr;
}

void KinectLiveFrameSource::FillFrame(KinectFrame* dst) {
  if (currentFrame.depthData) {
    CopyKinectFrame(&currentFrame, dst);
  }
}

int KinectLiveFrameSource::FrameNumber() const {
  return frameNumber.load(); 
}
