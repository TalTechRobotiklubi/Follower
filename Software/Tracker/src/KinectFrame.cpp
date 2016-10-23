#include "KinectFrame.h"
#include <assert.h>
#include <string.h>

void CopyKinectFrame(const KinectFrame* src, KinectFrame* dst) {
  assert(src->depthLength == dst->depthLength);
  if (src->depthData) {
    memcpy(dst->depthData, src->depthData, src->depthLength * sizeof(uint16_t));
  }
}
