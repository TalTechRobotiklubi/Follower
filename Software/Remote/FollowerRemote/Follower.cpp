#include "Follower.h"
#include "CANWorkerThread.h"
#include "SpineCommCAN.h"
#include "WorkerObjectCAN.h"
#include "DataLayerCAN.h"
#include "Kinematics.h"

Follower::Follower(void)
{
    workerThread_ = new CANWorkerThread();
    workerObject_ = new WorkerObjectCAN();
    workerObject_->moveToThread(workerThread_);
    kinematics_ = new Kinematics(workerObject_->getDataLayer());
    
  /*  QObject::connect(workerThread_, SIGNAL(started()), workerObject_, SLOT(process()));
    QObject::connect(workerThread_, SIGNAL(finished()), workerObject_, SLOT(end()));
    QObject::connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));*/
    QObject::connect(workerThread_, SIGNAL(finished()), workerObject_, SLOT(deleteLater()));
}

Follower::~Follower(void)
{
  delete kinematics_;
  workerThread_->wait();
  delete workerThread_;
}
