// Copyright 2016 TUT Robotics Club

#pragma once

#include <stdint.h>
#include "vec2.h"

struct CommInput {
  int16_t left_motor_speed;
  int16_t left_motor_encoder;
  int16_t right_motor_speed;
  int16_t right_motor_encoder;
  uint8_t distance_sensor1;
  uint8_t distance_sensor2;
  uint8_t distance_sensor3;
  uint8_t distance_sensor4;
  uint8_t distance_sensor5;
  uint8_t distance_sensor6;
  uint8_t distance_sensor7;
  uint8_t distance_sensor8;
  int32_t degreesToTurn;

  CommInput() {
    left_motor_speed = 0;
    left_motor_encoder = 0;
    right_motor_speed = 0;
    right_motor_encoder = 0;
    distance_sensor1 = 0;
    distance_sensor2 = 0;
    distance_sensor3 = 0;
    distance_sensor4 = 0;
    distance_sensor5 = 0;
    distance_sensor6 = 0;
    distance_sensor7 = 0;
    distance_sensor8 = 0;
    degreesToTurn = 0;
  }
};

