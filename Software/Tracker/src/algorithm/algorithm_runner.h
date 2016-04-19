// Copyright 2016 TUT Robotics Club

#pragma once

#include <vector>
#include <memory>

#include "ialgorithm.h"

class AlgorithmRunner {
 public:
   static void initialize();
   static void start(unsigned int id, const CommInput& in_data);
   static void run(unsigned int id, const CommInput& in_data, CommOutput* out_data);
   static void stop(unsigned int id);

 private:
   static std::vector<std::unique_ptr<IAlgorithm>> algorithms_;
};
