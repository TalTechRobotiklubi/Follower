#include "vec3.h"
#include <math.h>

vec3 vec3_normalize(vec3 v) {
  const float a = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
  return vec3{v.x / a, v.y / a, v.z / a};
}

float vec3_dot(vec3 a, vec3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

vec3 vec3_sub(vec3 a, vec3 b) {
  vec3 r;
  r.x = a.x - b.x;
  r.y = a.y - b.y;
  r.z = a.z - b.z;
  return r;
}

vec3 vec3_cross(vec3 u, vec3 v) {
  const float t1 = u.x - u.y;
  const float t2 = v.y + v.z;
  const float t3 = u.x * v.z;
  const float t4 = t1 * t2 - t3;

  vec3 r;
  r.x = v.y * (t1 - u.z) - t4;
  r.y = u.z * v.x - t3;
  r.z = t4 - u.y * (v.x - t2);

  return r;
}
