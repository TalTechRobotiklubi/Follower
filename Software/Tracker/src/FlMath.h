#pragma once
#include <stdint.h>

namespace Follower {
const float Pi = 3.14159265358979323846f;
}

struct vec2i {
  vec2i() : x(0), y(0) {}
  vec2i(int x, int y) : x(x), y(y) {}
  int x;
  int y;
};

struct vec2 {
  vec2() : x(0.f), y(0.f) {}
  vec2(float x, float y) : x(x), y(y) {}
  float x;
  float y;
};

struct vec3 {
  float x;
  float y;
  float z;

  vec3() : x(0.f), y(0.f), z(0.f) {}
  vec3(float x, float y, float z) : x(x), y(y), z(z) {}

  vec2 xz() const { return {x, z}; }
};

vec2 vec2_rotate(vec2 a, float rad);

inline float deg_to_rad(float deg) { return deg * (Follower::Pi / 180.f); }

float fl_map_range(float x, float a, float b, float c, float d);

template <typename T>
T fl_clamp(T v, T min, T max) {
  if (v < min) return min;
  if (v > max) return max;

  return v;
}
