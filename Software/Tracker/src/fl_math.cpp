#include "fl_math.h"

float fl_map_range(float x, float a, float b, float c, float d) {
  return (x - a) * (d - c) / (b - a) + c;
}

uint8_t fl_depth_to_byte(uint16_t value, uint16_t min, uint16_t max) {
  const uint32_t v = fl_clamp(value, min, max);
  const uint32_t lmin = min;
  const uint32_t lmax = max;
  return static_cast<uint8_t>((v - lmin) * 255 / (lmax - lmin));
}
