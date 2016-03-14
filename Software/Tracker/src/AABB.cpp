#include "AABB.h"
#include <assert.h>
#include <float.h>
#include <algorithm>

void aabb_expand(AABB* bbox, vec2 point) {
  if (point.x < bbox->top_left.x)
    bbox->top_left.x = point.x;
  else if (point.x > bbox->bot_right.x)
    bbox->bot_right.x = point.x;

  if (point.y < bbox->top_left.y)
    bbox->top_left.y = point.y;
  else if (point.y > bbox->bot_right.y)
    bbox->bot_right.y = point.y;
}

vec2 aabb_center(const AABB* bbox) {
  return {(bbox->bot_right.x + bbox->top_left.x) * 0.5f,
          (bbox->bot_right.y + bbox->top_left.y) * 0.5f};
}

bool aabb_overlap(const AABB* a, const AABB* b) {
  if (a->bot_right.x < b->top_left.x || a->top_left.x > b->bot_right.x)
    return false;
  if (a->bot_right.y < b->top_left.y || a->top_left.y > b->bot_right.y)
    return false;

  return true;
}

vec2 aabb_size(const AABB* a) {
  vec2 r;
  r.x = a->bot_right.x - a->top_left.x;
  r.y = a->bot_right.y - a->top_left.y;
  return r;
}

AABB aabb_from_points(const vec2* points, size_t len) {
  assert(len >= 2);

  AABB bbox = {{FLT_MAX, FLT_MAX}, {-FLT_MAX, -FLT_MAX}};

  for (size_t i = 0; i < len; i++) {
    aabb_expand(&bbox, points[i]);
  }

  return bbox;
}

