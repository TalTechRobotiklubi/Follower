#ifndef VEC3_H
#define VEC3_H

struct vec3 {
  float x;
  float y;
  float z;
};

vec3 vec3_normalize(vec3 v);
// a dot b
float vec3_dot(vec3 a, vec3 b);
// a - b
vec3 vec3_sub(vec3 a, vec3 b);
// a x b
vec3 vec3_cross(vec3 u, vec3 v);

#endif
