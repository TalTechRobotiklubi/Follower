#include "AABB.h"
#include <assert.h>
#include <float.h>

void aabb_expand(AABB* bbox, vec2 point) {
	if (point.x < bbox->top_left.x) bbox->top_left.x = point.x;
	else if (point.x > bbox->bot_right.x) bbox->bot_right.x = point.x;

	if (point.y < bbox->top_left.y) bbox->top_left.y = point.y;
	else if (point.y > bbox->bot_right.y) bbox->bot_right.y = point.y;
}

vec2 aabb_center(const AABB* bbox) {
  return {
    (bbox->bot_right.x + bbox->top_left.x) * 0.5f,
    (bbox->bot_right.y + bbox->top_left.y) * 0.5f
  };
}

AABB aabb_from_points(const vec2* points, size_t len) {

	assert(len >= 2);

	AABB bbox = {
		{ -FLT_MAX, -FLT_MAX },
		{ FLT_MAX, FLT_MAX }
	};

	for (size_t i = 0; i < len; i++) {
		aabb_expand(&bbox, points[i]);
	}

	return bbox;
}
