// Copyright 2016 TUT Robotics Club

#pragma once

#include "comm_output.h"

class IComm {
public:
  virtual ~IComm() {}
  virtual bool isOpen() const = 0;
  virtual void send(const CommOutput& data) = 0;
};

