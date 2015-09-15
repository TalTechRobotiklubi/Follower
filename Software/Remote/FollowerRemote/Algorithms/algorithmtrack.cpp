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
    uint16_t TrackedObjectX;  // 0 ... 512
    uint16_t TrackedObjectY;  // 0 ... 424
    uint16_t TrackedObjectHeight; // 0 ... 512
    uint16_t TrackedObjectWidth;  // 0 ... 424
    Execution exec;
    exec.leftSpeed = 0;
    exec.rightSpeed = 0;

    dataLayer->DL_getData(DLParamTrackedObjectX, &TrackedObjectX);
    dataLayer->DL_getData(DLParamTrackedObjectY, &TrackedObjectY);
    dataLayer->DL_getData(DLParamTrackedObjectHeight, &TrackedObjectHeight);
    dataLayer->DL_getData(DLParamTrackedObjectWidth, &TrackedObjectWidth);

    qDebug() << "TrackedObject X Y H W"<< TrackedObjectX << TrackedObjectY
           << TrackedObjectHeight << TrackedObjectWidth ;

    if (TrackedObjectX != 0)
    {
        int16_t error = TrackedObjectX - 256;
        const int16_t p = 30;
        if (error < -30) // object on left, turn to left
        {
            exec.leftSpeed = qMax(p * error, -600);
            exec.rightSpeed = qMin(p * qAbs(error), 600);
        }
        else if (error > 30) // object on right, turn to right
        {
            exec.leftSpeed = qMin(p * error, 600);
            exec.rightSpeed = qMax(p * -error, -600);
        }
    }

    return exec;
}

void AlgorithmTrack::stop()
{

}

