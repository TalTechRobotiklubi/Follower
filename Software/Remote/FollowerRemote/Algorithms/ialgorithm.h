#ifndef IALGORITHM
#define IALGORITHM

#include "DataLayerBase.h"

class IAlgorithm
{
public:
    struct Execution
    {
        int16_t xSpeed;
        int16_t wSpeed;

        Execution()
         : xSpeed(0), wSpeed(0)
        { }

        Execution(int16_t xSpeed, int16_t wSpeed)
        {
          this->xSpeed = xSpeed;
          this->wSpeed = wSpeed;
        }
    };
    virtual ~IAlgorithm() {}
    virtual void start() = 0;
    virtual Execution run() = 0;
    virtual void stop() = 0;
};

#endif // IALGORITHM

