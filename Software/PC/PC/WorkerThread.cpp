#include "WorkerThread.h"
#include "SpineCmdCAN.h"
#include "SpineDataCAN.h"
#include "SpineCommCAN.h"

WorkerThread::WorkerThread(QObject *parent)	: QThread(parent)
{
    spineCmd_	= new SpineCmdCAN();
	spineData_	= new SpineDataCAN();	
	spineComm_	= new SpineCommCAN();
    remoteSpineCmd_ = new SpineCmdCAN();
    remoteSpineData_ = new SpineDataCAN();
}


WorkerThread::~WorkerThread(void)
{
}

bool WorkerThread::Start()
{
    threadStopCmd_ = false;
	QThread::start();

	return true;
}

void WorkerThread::onNewData(bool newSpineData, SpineData* spineData)
{
    *remoteSpineData_ = *dynamic_cast<SpineDataCAN*>(spineData);

    // set attacking goal if any of the right side buttons have been pressed. Send info to LCD.
    if (remoteSpineData_->GetRightBottomButton() == true)
    {
        char* text = "Yel  ";
        ((SpineCmdCAN*)(spineCmd_))->SetScreenWriteCommand(3, 5, 5, text);
    }
    if (remoteSpineData_->GetRightUpButton() == true)
    {
        char* text = "Blue ";
        ((SpineCmdCAN*)(spineCmd_))->SetScreenWriteCommand(3, 5, 5, text);
    }
}

void WorkerThread::run()
{

}