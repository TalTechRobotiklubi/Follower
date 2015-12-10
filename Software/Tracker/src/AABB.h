#ifndef AABB_H
#define AABB_H

#include "vec2.h"
#include <cstddef>
#include <vector>

struct AABB {
  vec2 top_left;
  vec2 bot_right;
};

void aabb_expand(AABB* bbox, vec2 point);
vec2 aabb_center(const AABB* bbox);
bool aabb_overlap(const AABB* a, const AABB* b);

AABB aabb_from_points(const vec2* points, size_t len);

#endif
