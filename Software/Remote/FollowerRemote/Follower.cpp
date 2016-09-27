#include "Follower.h"
#include "CANWorkerThread.h"
#include "SpineCommCAN.h"
#include "WorkerObjectCAN.h"
#include "DataLayerCAN.h"
#include "Kinematics.h"

#include <QSettings>

Follower::Follower(void)
{
    settings_ = new QSettings("robot.ini", QSettings::IniFormat);
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
  workerThread_->exit();
  workerThread_->wait();
  delete workerThread_;
  delete settings_;
}
