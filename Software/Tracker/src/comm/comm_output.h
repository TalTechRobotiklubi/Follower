// Copyright 2016 TUT Robotics Club
#pragma once

#include <stdint.h>
#include "CoreObj.h"

struct CommOutput {
  int16_t translation_speed;
  int16_t rotation_speed;
  Vec2 camera_degrees;
  Activity activity;

  CommOutput() {
    translation_speed = 0;
    rotation_speed = 0;
    camera_degrees = {0, 0};
    activity = Resting;
  }
};
