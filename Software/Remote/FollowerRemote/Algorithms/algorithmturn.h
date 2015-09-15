#ifndef ALGORITHMTURN_H
#define ALGORITHMTURN_H

#include "ialgorithm.h"

class AlgorithmTurn : public IAlgorithm
{
public:
    AlgorithmTurn(DataLayerBase* dataLayer);

    void start() override;
    Execution run() override;
    void stop() override;

private:
    const int StopDistance = 60;

    int getSensorError();

    DataLayerBase* dataLayer;
    int counter = 0;
    int lastState = 0;
    int speed = 1000;
    int pidP;
    int pidI;
    int pidD;
    int Turn = 0;
    //int SavedTurn;
};

#endif // ALGORITHMTURN_H

