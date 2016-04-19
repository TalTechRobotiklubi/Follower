// Copyright 2016 TUT Robotics Club

#include "algorithm_runner.h"

#include "algorithm_move.h"

std::vector<std::unique_ptr<IAlgorithm>> AlgorithmRunner::algorithms_;

void AlgorithmRunner::initialize() {
  if (algorithms_.size() == 0)
  {
    algorithms_.push_back(std::unique_ptr<AlgorithmMove>(new AlgorithmMove())); // id == 0
  }
}

void AlgorithmRunner::start(unsigned int id, const CommInput& in_data) {
  if (id < algorithms_.size())
    algorithms_[id]->start(in_data);
}

void AlgorithmRunner::run(unsigned int id, const CommInput& in_data, CommOutput* out_data) {
  if (id < algorithms_.size() && algorithms_[id]->isRunning())
    algorithms_[id]->run(in_data, out_data);
}

void AlgorithmRunner::stop(unsigned int id) {
  if (id < algorithms_.size())
    algorithms_[id]->stop();
}