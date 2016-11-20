#include "KinectFrame.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void KinectFrameInit(KinectFrame* frame, int width, int height) {
  int len = width * height;
  frame->depthLength = len;
  frame->depthData = (uint16_t*)calloc(len, sizeof(uint16_t));

  frame->rgbaWidth = width;
  frame->rgbaHeight = height;
  frame->rgbaLength = len * 4;
  frame->rgbaData = (uint8_t*)calloc(frame->rgbaLength, 1);
}

void CopyKinectFrame(const KinectFrame* src, KinectFrame* dst) {
  assert(src->depthLength == dst->depthLength);
  if (src->depthData) {
    memcpy(dst->depthData, src->depthData, src->depthLength * sizeof(uint16_t));
  }

  if (src->rgbaData) {
    memcpy(dst->rgbaData, src->rgbaData, src->rgbaLength);
  }
}
