#pragma once
#include <stdint.h>

const float Pi = 3.14159265358979323846f;

struct Vec2i {
  Vec2i() : x(0), y(0) {}
  Vec2i(int x, int y) : x(x), y(y) {}
  int x;
  int y;
};

struct Vec2 {
  Vec2() : x(0.f), y(0.f) {}
  Vec2(float x, float y) : x(x), y(y) {}
  float x;
  float y;
};

struct Vec3 {
  float x;
  float y;
  float z;

  Vec3() : x(0.f), y(0.f), z(0.f) {}
  Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

  Vec2 xz() const { return {x, z}; }
};

Vec2 Vec2Rotate(Vec2 a, float rad);

inline float DegToRad(float deg) { return deg * (Pi / 180.f); }

float MapRange(float x, float a, float b, float c, float d);

template <typename T>
T Clamp(T v, T min, T max) {
  if (v < min) return min;
  if (v > max) return max;

  return v;
}
