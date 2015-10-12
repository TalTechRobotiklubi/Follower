#ifndef KALMAN_H
#define KALMAN_H

#include <Eigen/Dense>

struct kalman_filter {
  // x y dx dy
  Eigen::Vector4f state;
  Eigen::Matrix<float, 2, 4> H;
  Eigen::Matrix4f P;
  Eigen::Matrix2f R;
  Eigen::Matrix4f F;
  Eigen::Matrix4f Q;
};

// x y
void kalman_update(kalman_filter* filter, const Eigen::Vector2f& measurement);
Eigen::Vector4f kalman_predict(kalman_filter* filter);

#endif
