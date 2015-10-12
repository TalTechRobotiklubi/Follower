#ifndef FL_MATH_H
#define FL_MATH_H

#include <stdint.h>

float fl_map_range(float x, float a, float b, float c, float d);

template <typename T>
T fl_clamp(T v, T min, T max) {
	if (v < min) return min;
	if (v > max) return max;

	return v;
}

uint8_t fl_depth_to_byte(uint16_t value, uint16_t min, uint16_t max);

#endif
