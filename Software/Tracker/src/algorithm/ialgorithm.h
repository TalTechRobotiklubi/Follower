// Copyright 2016 TUT Robotics Club

#pragma once

#include "comm/comm_output.h"
#include "comm/comm_input.h"

class IAlgorithm
{
public:
    virtual ~IAlgorithm() {}
    virtual void start(const CommInput& in_data) = 0;
    virtual void run(const CommInput& in_data, CommOutput* out_data) = 0;
    virtual bool isRunning() = 0;
    virtual void stop() = 0;
};

