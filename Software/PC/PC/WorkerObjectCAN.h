#pragma once

#include "WorkerObjectBase.h"

class DataLayerCAN;
class SpineCommCAN;

class WorkerObjectCAN : public WorkerObjectBase
{
    Q_OBJECT
public:
    WorkerObjectCAN(SpineCommCAN* spineComm);
    ~WorkerObjectCAN(void);

    virtual void onStart();
    virtual void onStop();
    virtual DataLayerBase *getDataLayer();

private:
    SpineCommCAN*   spineCommCAN_;
};

