#ifndef IALGORITHM
#define IALGORITHM

#include "DataLayerBase.h"

class IAlgorithm
{
public:
    struct Execution
    {
        int leftSpeed;
        int rightSpeed;
    };
    virtual ~IAlgorithm() {}
    virtual void start() = 0;
    virtual Execution run() = 0;
    virtual void stop() = 0;
};

#endif // IALGORITHM

