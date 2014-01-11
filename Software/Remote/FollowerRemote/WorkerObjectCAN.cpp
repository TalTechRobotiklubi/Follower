#include "WorkerObjectCAN.h"
#include "DataLayerCAN.h"
#include "SpineCommCAN.h"

WorkerObjectCAN::WorkerObjectCAN(SpineCommCAN *spineComm)
    : WorkerObjectBase(spineComm)
{
    spineCommCAN_ = spineComm;
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
    return spineCommCAN_->getDataLayer();
}