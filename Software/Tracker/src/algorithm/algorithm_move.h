// Copyright 2016 TUT Robotics Club

#pragma once

#include <mutex>

#include "ialgorithm.h"

static const int Quaternion_Size = 4;

namespace {
struct ParsedData {
  int16_t qs[Quaternion_Size];
  int32_t timestamp;

  ParsedData() : timestamp(0) {
    for (int i = 0; i < Quaternion_Size; ++i)
      qs[i] = 0;
  }
};
} // namespace

class AlgorithmMove : public IAlgorithm {
 public:
  void start() override;
  void run(const CommInput& in_data, CommOutput* out_data) override;
  bool isRunning() override;
  void stop() override;

private:
  void parse(const char* buffer, size_t size);
  void pushDataToAlgorithm(const ParsedData& data);

  void calculateEulerDegrees();
  void runAlgorithm();

  bool is_running_ = false;
  ParsedData data_;
  std::mutex mutex_;
  double yaw_ = 0;
  double pitch_ = 0;
  double roll_ = 0;
};

