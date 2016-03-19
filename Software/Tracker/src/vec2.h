#ifndef __VEC2_H__
#define __VEC2_H__

struct vec2 {
  float x;
  float y;
};

vec2 vec2_sub(vec2 a, vec2 b);
vec2 vec2_normalize(vec2 v);
vec2 vec2_mul(vec2 a, float s);
vec2 vec2_mul_pcw(vec2 a, vec2 b);
float vec2_distance(vec2 a, vec2 b);
float vec2_length(vec2 v);

#endif
