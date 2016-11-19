#pragma once

struct vec2i {
  vec2i() : x(0), y(0) {}
  vec2i(int x, int y) : x(x), y(y) {}
  int x;
  int y;
};

struct vec2 {
  vec2() : x(0.f), y(0.f) {}
  vec2(float x, float y) : x(x), y(y) {}
  float x;
  float y;
};

vec2 vec2_sub(vec2 a, vec2 b);
vec2 vec2_normalize(vec2 v);
vec2 vec2_mul(vec2 a, float s);
vec2 vec2_mul_pcw(vec2 a, vec2 b);
vec2 vec2_rotate(vec2 a, float rad);
float vec2_dot(vec2 a, vec2 b);
float vec2_angle(vec2 a, vec2 b);
float vec2_distance(vec2 a, vec2 b);
float vec2_length(vec2 v);
