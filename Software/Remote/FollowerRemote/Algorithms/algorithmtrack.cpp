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
  int16_t speedX = 300;
  int16_t speedW = 0;

  return Execution(speedX, speedW);
}

void AlgorithmTrack::stop()
{

}

