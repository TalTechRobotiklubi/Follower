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

struct Euler {
  double yaw;
  double pitch;
  double roll;

  Euler() : yaw(0), pitch(0), roll(0) {}
};
} // namespace

class AlgorithmMove : public IAlgorithm {
 public:
   AlgorithmMove();
   ~AlgorithmMove();
  void start(const CommInput& in_data) override;
  void run(const CommInput& in_data, CommOutput* out_data) override;
  bool isRunning() override;
  void stop() override;

private:
  void parse(const char* buffer, size_t size);
  void pushDataToAlgorithm(const ParsedData& data);

  Euler calculateEulerDegrees();
  void runAlgorithm(const CommInput& in_data, CommOutput* out_data);

  bool is_running_ = false;
  ParsedData data_;
  std::mutex mutex_;
  Euler init_degrees_;
  Euler set_point_;
};

