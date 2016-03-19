#ifndef FL_CONSTANTS
#define FL_CONSTANTS

#include <stdint.h>

namespace fl {

const int KINECT_DEPTH_W = 512;
const int KINECT_DEPTH_H = 424;
const int KINECT_IR_W = 512;
const int KINECT_IR_H = 424;
const int DEPTH_RANGE_INTERVALS = 16;

const uint16_t MIN_RELIABLE_DIST = 500;
const uint16_t MAX_RELIABLE_DIST = 4500;

}

#endif
