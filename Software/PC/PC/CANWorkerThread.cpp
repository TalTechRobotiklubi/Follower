#include "CANWorkerThread.h"
#include "SpineCmdCAN.h"
#include "SpineDataCAN.h"
#include "SpineCommCAN.h"

#include <QDebug>

CANWorkerThread::CANWorkerThread(void)
{
	serialPortNumber_ = 5;
}


CANWorkerThread::~CANWorkerThread(void)
{
}


void CANWorkerThread::initData(void)
{
    spineCmd_	= new SpineCmdCAN();
	spineData_	= new SpineDataCAN();	
	spineComm_	= new SpineCommCAN();
    remoteSpineCmd_ = new SpineCmdCAN();
    remoteSpineData_ = new SpineDataCAN();
}

void CANWorkerThread::onNewSpineCmd(SpineCmd* spineCmd)
{
    dynamic_cast<SpineCommCAN*>(spineComm_)->SetLogicDataToDataLayer(spineCmd);
}

void CANWorkerThread::onNewData(bool newSpineData, SpineData* spineData)
{
    *remoteSpineData_ = *dynamic_cast<SpineDataCAN*>(spineData);
    emit update_UIdata(remoteSpineData_);
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
    int sensors[6];
    remoteSpineData_->GetSensorData(sensors);
    qDebug() << sensors[0] << sensors[1];
}

void CANWorkerThread::onAlgorithmStart()
{
    qDebug() << "Start";
}

void CANWorkerThread::onAlgorithmStop()
{
	qDebug() << "Stop";
}