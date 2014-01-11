#include "Follower.h"
#include "CANWorkerThread.h"
#include "SpineCommCAN.h"
#include "WorkerObjectCAN.h"
#include "DataLayerCAN.h"

Follower::Follower(void)
{
    workerThread_ = new CANWorkerThread();
    spineComm_ = new SpineCommCAN();
    workerObject_ = new WorkerObjectCAN(spineComm_);
    workerObject_->moveToThread(workerThread_);
    
  /*  QObject::connect(workerThread_, SIGNAL(started()), workerObject_, SLOT(process()));
    QObject::connect(workerThread_, SIGNAL(finished()), workerObject_, SLOT(end()));
    QObject::connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));*/
}


Follower::~Follower(void)
{
    delete workerObject_;
    delete workerThread_;
    delete spineComm_;
}
