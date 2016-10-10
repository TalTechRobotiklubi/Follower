#include "fl_math.h"

float fl_map_range(float x, float a, float b, float c, float d) {
  return (x - a) * (d - c) / (b - a) + c;
}
