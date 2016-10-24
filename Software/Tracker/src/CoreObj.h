#pragma once
#include <stdint.h>
#include "vec3.h"

struct Detection {
  vec2 kinectPosition;
  vec3 metricPosition;
  float weight;
};

struct World {
  double timestamp = 0.0;
  int32_t numDetections = 0;
  Detection detections[16];
};

struct Target {
  float weight = 0.f;
  vec2 kinect;
  vec3 position;

  Target() = default;
  Target(float weight, vec2 kinect, vec3 position)
    : weight(weight), kinect(kinect), position(position) {}
};

struct TrackingState {
  int32_t activeTarget = -1;
  int32_t numTargets = 0;
  Target targets[16];
};

struct ControlState {
  vec2 camera;
  float rotationSpeed = 0.f;
  float speed = 0.f;
};
