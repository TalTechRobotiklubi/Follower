#include "WorkerThreadBase.h"

#include "SpineData.h"
#include "SpineCmd.h"
#include "CSpineComm.h"

#include <QElapsedTimer>

#include <assert.h>

#include <iostream>


WorkerThreadBase::WorkerThreadBase(QObject *parent)	: QThread(parent)
{
	dataInited_			= false;
    serialPortNumber_   = 0;
    periodOver_         = true;
}

bool WorkerThreadBase::Start()
{
	threadStopCmd_ = false;
	QThread::start();

	return true;
}

#if 0
void WorkerThreadBase::run()
{
    exec();

	if (!dataInited_)
	{
		initData();
		dataInited_ = true;
	}

	if(!spineComm_->IsOpen() && !spineComm_->Open(serialPortNumber_))
	{
		
		return;
	}

	newRemoteSpineCmd_	= false;

	bool prevStartButtonState = true;

	QElapsedTimer timer;
	timer.start();

	onSpineConnected();

    // start main loop periodic timer
    timerId_ = startTimer(10);

	do 
	{
    #if 0
		spineComm_->Communicate();

		newSpineData_ = spineComm_->HasNewData();

		if(newSpineData_)
		{
			spineComm_->GetData(spineData_);
			spineComm_->ClearNewData();
		}

		double secsElapsed = timer.elapsed() / 1000.0;

		if (newSpineData_)
		{
			spineData_->SetTime(secsElapsed);
		}

		bool algoStateChanged = false;

		//
		// Call virtual method onNewData().
		// Derived class can do data recording in overwritten method
		//
		if (newSpineData_)
		{
			onNewData(newSpineData_, spineData_);
		}

		prevStartButtonState = spineData_->GetStartButton();
		
		newSpineData_	= false;

		if(newRemoteSpineCmd_)
		{
			QMutexLocker lock(&remoteSpineCmdMutex_);

			spineCmd_->Copy(remoteSpineCmd_);
			newRemoteSpineCmd_	= false;
		}

		if(spineCmd_->NewCommands())
		{
			//
			// emit requested speed (to UI?)
			// Call onNewSpineCmd() for processing by derived class
			//
			emit speedCmdChanged(spineCmd_->GetAbsSpeed(), spineCmd_->GetRadAngle(), spineCmd_->GetRadTurnRate());
			onNewSpineCmd(spineCmd_);

			//
			// Send requested speed to motor controller
			//
			if(!spineComm_->SendControllerCommands(spineCmd_)) 
			{
				break;
			}
		}

		spineCmd_->ClearCommands();
        #endif
        if (periodOver_)
        {
            periodOver_ = false;
            // do algorithm
            // ....
            spineComm_->SendControllerCommands();
        }
        else
        {
            // check received data
            spineComm_->Communicate();
        }
	}
	while(spineComm_->IsOpen() && !threadStopCmd_);

    killTimer(timerId_);

	onSpineDisconnected();

	if(spineComm_->IsOpen())
	{
		spineCmd_->Stop();
		spineComm_->SendControllerCommands();
	}

}
#endif

void WorkerThreadBase::Stop()
{
    emit stopped();
	threadStopCmd_ = true;
    exit();
    wait();
}

#if 0
void WorkerThreadBase::SendSpineCmd( SpineCmd* spineCmd )
{
	if (serialPortNumber_ == 0) return;

	QMutexLocker lock(&remoteSpineCmdMutex_);

	remoteSpineCmd_->Copy(spineCmd);
	newRemoteSpineCmd_	= true;
}
#endif


WorkerThreadBase::~WorkerThreadBase()
{
}