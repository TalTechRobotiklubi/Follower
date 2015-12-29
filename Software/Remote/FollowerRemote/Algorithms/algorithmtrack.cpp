#include "algorithmtrack.h"

#include <QtGlobal>

#include <QDebug>

AlgorithmTrack::AlgorithmTrack(DataLayerBase* dataLayer)
{
    this->dataLayer = dataLayer;
}

void AlgorithmTrack::start()
{

}

IAlgorithm::Execution AlgorithmTrack::run()
{
    return Execution();
}

void AlgorithmTrack::stop()
{

}

