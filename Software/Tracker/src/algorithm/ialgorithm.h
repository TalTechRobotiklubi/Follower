// Copyright 2016 TUT Robotics Club

#pragma once

#include "comm/comm_output.h"

class IAlgorithm
{
public:
    virtual ~IAlgorithm() {}
    virtual void start() = 0;
    virtual void run(CommOutput* outData) = 0;
    virtual bool isRunning() = 0;
    virtual void stop() = 0;
};

