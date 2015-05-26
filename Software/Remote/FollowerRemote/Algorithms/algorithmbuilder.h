#ifndef ALGORITHMBUILDER_H
#define ALGORITHMBUILDER_H

#include "ialgorithm.h"

class AlgorithmBuilder
{
public:
    static IAlgorithm* create(int number, DataLayerBase* dataLayer);
};

#endif // ALGORITHMBUILDER_H
