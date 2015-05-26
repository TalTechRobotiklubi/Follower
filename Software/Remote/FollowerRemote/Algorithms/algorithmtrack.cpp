#include "algorithmtrack.h"

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
    uint16_t TrackedObjectX;
    uint16_t TrackedObjectY;
    uint16_t TrackedObjectHeight;
    uint16_t TrackedObjectWidth;

    dataLayer->DL_getData(DLParamTrackedObjectX, &TrackedObjectX);
    dataLayer->DL_getData(DLParamTrackedObjectY, &TrackedObjectY);
    dataLayer->DL_getData(DLParamTrackedObjectHeight, &TrackedObjectHeight);
    dataLayer->DL_getData(DLParamTrackedObjectWidth, &TrackedObjectWidth);

    qDebug() << "TrackedObject X Y H W"<< TrackedObjectX << TrackedObjectY
           << TrackedObjectHeight << TrackedObjectWidth ;

    Execution exec;
    exec.leftSpeed = 0;
    exec.rightSpeed = 0;
    return exec;
}

void AlgorithmTrack::stop()
{

}

