#include "vec2.h"
#include <cmath>

vec2 vec2_sub(vec2 a, vec2 b) {
  vec2 r;
  r.x = a.x - b.x;
  r.y = a.y - b.y;
  return r;
}

vec2 vec2_normalize(vec2 v) {
  const float a = sqrtf(v.x * v.x + v.y * v.y);
  return vec2{v.x / a, v.y / a};
}

vec2 vec2_mul(vec2 a, float s) { return {a.x * s, a.y * s}; }

vec2 vec2_mul_pcw(vec2 a, vec2 b) { return {a.x * b.x, a.y * b.y}; }

vec2 vec2_rotate(vec2 a, float rad) {
  vec2 res;
  res.x = a.x * cos(rad) - a.y * sin(rad);
  res.y = a.x * sin(rad) + a.y * cos(rad);
  return res;
}

float vec2_dot(vec2 a, vec2 b) { return a.x * b.x + a.y * b.y; }

float vec2_distance(vec2 a, vec2 b) {
  return std::hypotf(b.x - a.x, b.y - a.y);
}

float vec2_length(vec2 v) { return sqrtf(v.x * v.x + v.y * v.y); }

float vec2_angle(vec2 a, vec2 b) {
  float arc = (a.x * a.x + a.y * a.y) * (b.x * b.x + b.y * b.y);
  arc = sqrt(arc);
  if (arc > 0.f) {
    arc = acos(vec2_dot(a, b) / arc);
    if (a.x * b.y - a.y * b.x < 0.f) {
      arc = -arc;
    }
  }

  return arc;
}
