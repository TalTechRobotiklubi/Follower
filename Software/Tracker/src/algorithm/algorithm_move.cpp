// Copyright 2016 TUT Robotics Club

#include "algorithm_move.h"

void AlgorithmMove::start(const CommInput& in_data) {
  is_running_ = true;
}

void AlgorithmMove::run(const CommInput& in_data, CommOutput* out_data) {

}

bool AlgorithmMove::isRunning() {
  return is_running_;
}

void AlgorithmMove::stop() {
  is_running_ = false;
}
