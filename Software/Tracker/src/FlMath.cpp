#include "FlMath.h"
#include <cmath>

float fl_map_range(float x, float a, float b, float c, float d) {
  return (x - a) * (d - c) / (b - a) + c;
}

vec2 vec2_rotate(vec2 a, float rad) {
  vec2 res;
  res.x = a.x * cos(rad) - a.y * sin(rad);
  res.y = a.x * sin(rad) + a.y * cos(rad);
  return res;
}
