#ifndef __VEC2_H__
#define __VEC2_H__

struct vec2 {
	float x;
	float y;
};

vec2 vec2_mul(vec2 a, float s);
float vec2_distance(vec2 a, vec2 b);

#endif
