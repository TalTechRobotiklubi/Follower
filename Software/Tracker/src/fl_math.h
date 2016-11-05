#pragma once
#include <stdint.h>
#include "vec3.h"

namespace Follower {
  const float Pi = 3.14159265358979323846f;
}

inline float deg_to_rad(float deg) {
  return deg * (Follower::Pi / 180.f);
}

float fl_map_range(float x, float a, float b, float c, float d);

template <typename T>
T fl_clamp(T v, T min, T max) {
	if (v < min) return min;
	if (v > max) return max;

	return v;
}
