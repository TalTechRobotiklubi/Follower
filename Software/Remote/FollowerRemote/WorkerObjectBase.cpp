#include "WorkerObjectBase.h"
#include "CSpineComm.h"
#include "DataLayerBase.h"
#include <qdebug.h>
#include <QString>

WorkerObjectBase::WorkerObjectBase(SpineComm* spineComm)
    : spineComm_(spineComm), running_(false)
{
    
}

WorkerObjectBase::~WorkerObjectBase(void)
{
   
}

void WorkerObjectBase::startCommunication(QString nPort)
{
  onStart();

  if (spineComm_->open(nPort))
  {
      qDebug() << "Open success";
      timerId_ = startTimer(spineComm_->getPeriodInMs());
      running_ = true;
  }
  else
  {
      qDebug() << "Open failed";
      running_ = false;
  }
  emit startStatus(running_);
}

void WorkerObjectBase::stopCommunication(void)
{  
    running_ = false;
    killTimer(timerId_);
    spineComm_->close();
    onStop();
    qDebug() << "closing ";
    emit stopStatus();
}

void WorkerObjectBase::timerEvent(QTimerEvent*)
{   
   if (spineComm_->communicate())
      emit newDataReceived();
   spineComm_->sendControllerCommands();
}
