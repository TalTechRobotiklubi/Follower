#pragma once
#include "vec3.h"

struct Target {
  double weight = 1.0;
  vec2 kinectPosition;
  vec3 metricPosition;

  Target() = default;
  Target(double weight, vec2 kinectPos, vec3 metricPos)
    : weight(weight), kinectPosition(kinectPos), metricPosition(metricPos) {}
};


