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
  double weight = 1.0;
  vec2 kinectPosition;
  vec3 metricPosition;

  Target() = default;
  Target(double weight, vec2 kinectPos, vec3 metricPos)
    : weight(weight), kinectPosition(kinectPos), metricPosition(metricPos) {}
};

struct TrackingState {
  int32_t numTargets = 0;
  Target targets[16];
};

struct ControlState {
  vec2 camera;
  float rotationSpeed = 0.f;
  float speed = 0.f;
};
