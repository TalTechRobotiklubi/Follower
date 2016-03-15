#include "WorkerObjectCAN.h"
#include "DataLayerCAN.h"

WorkerObjectCAN::WorkerObjectCAN()
    : WorkerObjectBase(&spineComm_)
{
}

WorkerObjectCAN::~WorkerObjectCAN(void)
{
}

void WorkerObjectCAN::onStart(void)
{ 
}

void WorkerObjectCAN::onStop(void)
{
}

DataLayerBase *WorkerObjectCAN::getDataLayer()
{
  return spineComm_.getDataLayer();
}
