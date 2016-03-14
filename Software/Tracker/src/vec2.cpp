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

float vec2_distance(vec2 a, vec2 b) {
  return std::hypotf(b.x - a.x, b.y - a.y);
}

float vec2_length(vec2 v) {
  return sqrtf(v.x * v.x + v.y * v.y);
}
