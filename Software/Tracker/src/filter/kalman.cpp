#include "kalman.h"

void kalman_update(kalman_filter* filter, const Eigen::Vector2f& measurement) {
  auto y = measurement - filter->H * filter->state;
  auto S = filter->H * filter->P * filter->H.transpose() + filter->R;
  auto K = filter->P * filter->H.transpose() * S.inverse();
  filter->state = filter->state + (K * y);
  filter->P = (Eigen::Matrix4f::Identity() - K * filter->H) * filter->P;
}

Eigen::Vector4f kalman_predict(kalman_filter* filter) {
  filter->state = filter->F * filter->state; // +u
  filter->P = filter->F * filter->P * filter->F.transpose() + filter->Q;
  return filter->state;
}
