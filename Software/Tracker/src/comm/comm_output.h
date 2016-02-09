// Copyright 2016 TUT Robotics Club

#pragma once

#include <stdint.h>
#include "vec2.h"

struct CommOutput {
  int16_t left_speed;
  int16_t right_speed;
  vec2 camera_degrees;
};

