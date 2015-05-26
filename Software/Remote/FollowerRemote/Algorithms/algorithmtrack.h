#ifndef ALGORITHMTRACK_H
#define ALGORITHMTRACK_H

#include "ialgorithm.h"

class AlgorithmTrack : public IAlgorithm
{
public:
    AlgorithmTrack(DataLayerBase* dataLayer);

    void start() override;
    Execution run() override;
    void stop() override;

private:
    DataLayerBase* dataLayer;
};

#endif // ALGORITHMTRACK_H
