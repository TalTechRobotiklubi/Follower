#include "plane.h"

float plane_point_dist(plane p, vec3 q) {
  return (vec3_dot(p.n, q) - p.d) / vec3_dot(p.n, p.n);
}

plane plane_construct(vec3 a, vec3 b, vec3 c) {
  plane p;
  p.n = vec3_normalize(vec3_cross(vec3_sub(b, a), vec3_sub(c, a)));
  p.d = vec3_dot(p.n, a);
  return p;
}

