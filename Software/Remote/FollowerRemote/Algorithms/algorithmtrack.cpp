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
    static int16_t validityX = 0;
    uint16_t TrackedObjectX;  // 0 ... 512
//    uint16_t TrackedObjectY;  // 0 ... 424
//    uint16_t TrackedObjectHeight; // 0 ... 512
//    uint16_t TrackedObjectWidth;  // 0 ... 424
    Execution exec;
    exec.leftSpeed = 0;
    exec.rightSpeed = 0;

    if (dataLayer->DL_getData(DLParamTrackedObjectX, &TrackedObjectX))
        validityX = 20;
    else
        --validityX;
//    dataLayer->DL_getData(DLParamTrackedObjectY, &TrackedObjectY);
//    dataLayer->DL_getData(DLParamTrackedObjectHeight, &TrackedObjectHeight);
//    dataLayer->DL_getData(DLParamTrackedObjectWidth, &TrackedObjectWidth);

    const int speed = 400;

    qDebug() << "TrackedObject X"<< TrackedObjectX << validityX;
    if (validityX > 0)
    {
        int16_t error = TrackedObjectX - 256;
       // const int16_t p = 30;
        if (error < -30) // object on left, turn to left
        {
            exec.leftSpeed = -speed;
            exec.rightSpeed = speed;
        }
        else if (error > 30) // object on right, turn to right
        {
            exec.leftSpeed = speed;
            exec.rightSpeed = -speed;
        }
    }
    else if (validityX < 0)
        validityX = 0;

    return exec;
}

void AlgorithmTrack::stop()
{

}

