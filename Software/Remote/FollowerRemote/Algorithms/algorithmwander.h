#ifndef ALGORITHMWANDER_H
#define ALGORITHMWANDER_H

#include "ialgorithm.h"

class AlgorithmWander : public IAlgorithm
{
public:
    AlgorithmWander(DataLayerBase* dataLayer);

    void start() override;
    Execution run() override;
    void stop() override;

private:
    const int StopDistance = 50;

    int getSensorError();

    DataLayerBase* dataLayer;
    int counter = 0;
    int lastState = 0;
    int speed = 1200;
    int pidP;
    int pidI;
    int pidD;
};

#endif // ALGORITHMWANDER_H
