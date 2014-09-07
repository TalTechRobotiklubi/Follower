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

	wchar_t portName[6];

	if (nPort.length() > 5)
	{
		nPort.right(5);
	}
	
	int i;

	for (i = 0 ; i < nPort.length() ; i++)
	{
		portName[i] = wchar_t (nPort.at(i).unicode());
	}

	portName[i] = 0;

    if (spineComm_->Open(portName))
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