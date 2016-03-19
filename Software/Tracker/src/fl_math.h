#ifndef FL_MATH_H
#define FL_MATH_H

#include <stdint.h>
#include "vec3.h"

const float F_PI = 3.14159265358979323846f;
const float F_PI_2 = F_PI * 0.5f;
const float F_PI_4 = F_PI * 0.25f;

float fl_map_range(float x, float a, float b, float c, float d);

template <typename T>
T fl_clamp(T v, T min, T max) {
	if (v < min) return min;
	if (v > max) return max;

	return v;
}

vec3 pcl_normal(const vec3* points, int len);

float fl_atan(float x);

#endif
