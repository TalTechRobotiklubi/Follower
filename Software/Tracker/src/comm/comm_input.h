// Copyright 2016 TUT Robotics Club

#pragma once

#include <stdint.h>
#include "vec2.h"

#define NUM_OF_DISTANCE_SENSORS 8

struct CommInput {
  uint8_t distance_sensor[NUM_OF_DISTANCE_SENSORS];

  CommInput() {
    for (int i = 0; i < NUM_OF_DISTANCE_SENSORS; ++i)
        distance_sensor[i] = 0;
  }
};

