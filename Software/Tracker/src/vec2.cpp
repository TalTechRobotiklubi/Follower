#include "vec2.h"
#include <cmath>

vec2 vec2_mul(vec2 a, float s) {
  return {
    a.x * s,
    a.y * s
  };
}

float vec2_distance(vec2 a, vec2 b) {
  return std::hypotf(b.x - a.x, b.y - a.y);
}
