#include "FlMath.h"
#include <cmath>

float MapRange(float x, float a, float b, float c, float d) {
  return (x - a) * (d - c) / (b - a) + c;
}

Vec2 Vec2Rotate(Vec2 a, float rad) {
  return {a.x * cos(rad) - a.y * sin(rad), a.x * sin(rad) + a.y * cos(rad)};
}
