#include "WorkerObjectBase.h"
#include "CSpineComm.h"
#include "DataLayerBase.h"
#include <qdebug.h>

WorkerObjectBase::WorkerObjectBase(SpineComm* spineComm)
    : spineComm_(spineComm), running_(false)
{
    
}


WorkerObjectBase::~WorkerObjectBase(void)
{
   
}



void WorkerObjectBase::startCommunication(void)
{
    onStart();
    if (spineComm_->Open(5))
    {   
        qDebug() << "Open success";
        timerId_ = startTimer(spineComm_->GetPeriodInMs());
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
    bool result = spineComm_->Close();
    onStop();
    qDebug() << "closing " << result;
    emit stopStatus(result);
}


void WorkerObjectBase::newCommandsToSend()
{
    //spineComm_->SendControllerCommands();
}

void WorkerObjectBase::timerEvent(QTimerEvent *event)
{   
     spineComm_->Communicate();
     
     if (spineComm_->HasNewData())
     {
        emit newDataReceived();
     }
     // send periodic messages
     spineComm_->SendControllerCommands();
}