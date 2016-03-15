#pragma once

#include "WorkerObjectBase.h"
#include "SpineCommCAN.h"

class WorkerObjectCAN : public WorkerObjectBase
{
    Q_OBJECT
public:
    WorkerObjectCAN();
    ~WorkerObjectCAN(void);

    virtual void onStart();
    virtual void onStop();
    virtual DataLayerBase *getDataLayer();

private:
    SpineCommCAN spineComm_;
};

