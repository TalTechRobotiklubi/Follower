#ifndef PLANE_H
#define PLANE_H

#include "vec3.h"

struct plane {
  vec3 n;
  float d;
};

float plane_point_dist(plane p, vec3 q);
plane plane_construct(vec3 a, vec3 b, vec3 c);

#endif
