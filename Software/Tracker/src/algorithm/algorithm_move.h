// Copyright 2016 TUT Robotics Club

#pragma once

#include "ialgorithm.h"

class AlgorithmMove : public IAlgorithm {
 public:

  void start() override;
  void run(CommOutput* outData) override;
  void stop() override;
};

