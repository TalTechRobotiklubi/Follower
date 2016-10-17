#pragma once
#include "vec3.h"

struct Target {
  double timeToLive = 3.0;
  vec2 kinectPosition;
  vec3 metricPosition;

  Target() = default;
  Target(double ttl, vec2 kinectPos, vec3 metricPos)
    : timeToLive(ttl), kinectPosition(kinectPos), metricPosition(metricPos) {}
};


