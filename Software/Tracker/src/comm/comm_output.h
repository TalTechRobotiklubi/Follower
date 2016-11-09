// Copyright 2016 TUT Robotics Club

#pragma once

#include <stdint.h>
#include "vec2.h"
#include "CoreObj.h"

struct CommOutput {
  int16_t translation_speed;
  int16_t rotation_speed;
  vec2 camera_degrees;
  Activity activity;

  CommOutput() {
    translation_speed = 0;
    rotation_speed = 0;
    camera_degrees = { 0, 0 };
    activity = Resting;
  }
};

