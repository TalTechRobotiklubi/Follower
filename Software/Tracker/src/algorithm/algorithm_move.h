// Copyright 2016 TUT Robotics Club

#pragma once

#include "ialgorithm.h"

class AlgorithmMove : public IAlgorithm {
 public:
  void start(const CommInput& in_data) override;
  void run(const CommInput& in_data, CommOutput* out_data) override;
  bool isRunning() override;
  void stop() override;

private:
  bool is_running_ = false;
};

