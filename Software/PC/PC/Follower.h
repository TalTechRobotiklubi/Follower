#pragma once
#include "qobject.h"

class WorkerThreadBase;
class WorkerObjectBase;
class DataLayerBase;
class SpineCommCAN;

class Follower :
    public QObject
{
public:
    Follower(void);
    ~Follower(void);

    WorkerThreadBase    *getWorkerThread() { return workerThread_; }
    WorkerObjectBase    *getWorkerObject() { return workerObject_; }
private:
	WorkerThreadBase    *workerThread_;
    WorkerObjectBase    *workerObject_;
    SpineCommCAN        *spineComm_;
};

