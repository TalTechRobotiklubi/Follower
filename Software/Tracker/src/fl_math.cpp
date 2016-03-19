#include "fl_math.h"
#include <Eigen/Dense>

float det_3x3(const float* m) {
  vec3 u = { m[0], m[1], m[2] };
  vec3 v = { m[3], m[4], m[5] };
  vec3 w = { m[6], m[7], m[8] };

  return vec3_dot(u, vec3_cross(v, w));
}

vec3 lineq_solve_3x3(const float* a, const float* b) {
  const float mat_x[9] = { b[0], a[1], a[2], b[1], a[4], a[5], b[2], a[7], a[8] };
  const float mat_y[9] = { a[0], b[0], a[2], a[3], b[1], a[5], a[6], b[2], a[8] };
  const float mat_z[9] = { a[0], a[1], b[0], a[3], a[4], b[1], a[6], a[7], b[2] };

  const float d = det_3x3(a);
  const float x = det_3x3(mat_x) / d;
  const float y = det_3x3(mat_y) / d;
  const float z = det_3x3(mat_z) / d;

  return vec3{x, y, z};
}

float fl_map_range(float x, float a, float b, float c, float d) {
  return (x - a) * (d - c) / (b - a) + c;
}

vec3 pcl_normal(const vec3* points, int len) {
  Eigen::Matrix3f A = Eigen::Matrix3f::Zero();
  Eigen::Vector3f b = Eigen::Vector3f::Zero();
  for (int i = 0; i < len; i++) {
    const vec3 v = points[i];
    A(0, 0) += v.x * v.x;
    A(0, 1) += v.x * v.y;
    A(0, 2) += v.x;
    A(1, 0) += v.x * v.y;
    A(1, 1) += v.y * v.y;
    A(1, 2) += v.y;
    A(2, 0) += v.x;
    A(2, 1) += v.y;

    b(0) += v.x * v.z;
    b(1) += v.y * v.z;
    b(2) += v.z;
  }

  A(2, 2) += float(len);
  Eigen::Vector3f solution = A.llt().solve(b);
  return vec3_normalize(vec3{solution(0), solution(1), solution(2)});
}

float fl_atan(float x) {
  return F_PI_4 * x - x * (fabsf(x) - 1.f) * (0.2447f + 0.0663 * fabsf(x));
}
