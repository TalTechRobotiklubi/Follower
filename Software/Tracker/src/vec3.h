#pragma once

struct vec3 {
  float x;
  float y;
  float z;

  vec3() : x(0.f), y(0.f), z(0.f) {}
  vec3(float x, float y, float z) : x(x), y(y), z(z) {}
};

vec3 vec3_normalize(vec3 v);
// a dot b
float vec3_dot(vec3 a, vec3 b);
// a - b
vec3 vec3_sub(vec3 a, vec3 b);
// a x b
vec3 vec3_cross(vec3 u, vec3 v);
