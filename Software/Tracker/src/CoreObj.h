#pragma once
#include <stdint.h>
#include "FlMath.h"
#include "Image.h"
#include "comm/comm_input.h"

struct Detection {
  Vec2i depthTopLeft;
  Vec2i depthBotRight;
  Vec3 metricPosition;
  float weight;
  float histogram[768];
  RgbaImage color;
};

struct World {
  double timestamp = 0.0;
  int32_t numDetections = 0;
  Detection detections[16];
  uint8_t distance_sensors[NUM_OF_DISTANCE_SENSORS];
};

struct Target {
  float weight = 0.f;
  Vec2 kinect;
  Vec3 position;
  int32_t detectionIndex = -1;

  Target() = default;
  Target(float weight, Vec2 kinect, Vec3 position)
      : weight(weight), kinect(kinect), position(position) {}
};

enum Activity {
  Resting = 0,
  Searching = 1,
  Following = 2,
};

struct TrackingState {
  int32_t activeTarget = -1;
  int32_t numTargets = 0;
  Target targets[16];
  int32_t activity;
};

struct ControlState {
  Vec2 camera;
  float rotationSpeed = 0.f;
  float speed = 0.f;
};
