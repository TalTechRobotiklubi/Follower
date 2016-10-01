// Copyright 2016 TUT Robotics Club

#pragma once

#include <stdint.h>
#include "vec2.h"

struct CommInput {
  uint8_t distance_sensor1;
  uint8_t distance_sensor2;
  uint8_t distance_sensor3;
  uint8_t distance_sensor4;
  uint8_t distance_sensor5;
  uint8_t distance_sensor6;
  uint8_t distance_sensor7;
  uint8_t distance_sensor8;

  CommInput() {
    distance_sensor1 = 0;
    distance_sensor2 = 0;
    distance_sensor3 = 0;
    distance_sensor4 = 0;
    distance_sensor5 = 0;
    distance_sensor6 = 0;
    distance_sensor7 = 0;
    distance_sensor8 = 0;
  }
};

