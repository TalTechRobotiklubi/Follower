#include <QKeyEvent>
#include "followerui.h"
//#include "SpineCmdCAN.h"
//#include "SpineDataCAN.h"
#include "WorkerThreadBase.h"
#include "WorkerObjectBase.h"
#include "DataLayerBase.h"
#include "TRobot.h"
#include <qdebug.h>

FollowerUi::FollowerUi(Follower *robot)
	: QMainWindow()
{
	ui.setupUi(this); 
	workerThread_ = robot->getWorkerThread();
    workerObject_ = robot->getWorkerObject();
    dataLayer_ = robot->getWorkerObject()->getDataLayer();

    workerObject_->moveToThread(workerThread_);
    connect(this, SIGNAL(startCommunication()), workerObject_, SLOT(startCommunication()));
    connect(workerObject_, SIGNAL(startStatus(bool)), this, SLOT(startCommStatus(bool)));
    connect(workerObject_, SIGNAL(newDataReceived()), this, SLOT(newUiData()));
    connect(this, SIGNAL(newCommands()), workerObject_, SLOT(newCommandsToSend()));
    connect(this, SIGNAL(stopCommunication()), workerObject_, SLOT(stopCommunication()));
    connect(workerObject_, SIGNAL(stopStatus(bool)), this, SLOT(stopCommStatus(bool)));
    connect(ui.btnConnect, SIGNAL(clicked()), this, SLOT(connectSpine()));

	robotgui_ = new TRobot();
	scene_ = new QGraphicsScene(this);
	scene_->setBackgroundBrush(QBrush(Qt::black));
	ui.graphicsView->setScene(scene_);
	ui.graphicsView->setRenderHint(QPainter::Antialiasing);
	scene_->addItem(robotgui_);

}

FollowerUi::~FollowerUi()
{

	scene_->clear();
	robotgui_ = NULL; 
	delete scene_;

}

void FollowerUi::connectSpine() {

	if (workerThread_->isRunning())
    {
        /*workerThread_->Stop();
		workerThread_->quit();
        workerThread_->wait();
        ui.btnConnect->setText("Connect");*/
        emit stopCommunication();
	}
    else
    {
		workerThread_->start();
        // open communication
        emit startCommunication();
	}
/*
	_sleep(1000);

	if (workerThread_->isRunning()){
		ui.btnConnect->setText("Disconnect");
	}else{
		ui.btnConnect->setText("Connect");
	}*/
    //workerThread_->start();
}

void FollowerUi::startCommStatus(bool status)
{
    //connection failed. Stop thread.
    if (!status)
    {
        //workerThread_->Stop();
        workerThread_->exit();
        workerThread_->wait();
    }
    else
    {
        // set feedback to user that connection was ok
        ui.btnConnect->setText("Disconnect");
    }

}

void FollowerUi::stopCommStatus(bool status)
{
    //stop thread anyway.
    workerThread_->exit();
    workerThread_->wait();
    // set feedback to user that disconnection was ok
    ui.btnConnect->setText("Connect");
}

#if 0
void FollowerUi::newUiData(SpineData* spineData) {
    int sensors[8];

    SpineDataCAN* spineDataCAN = dynamic_cast<SpineDataCAN*>(spineData);
	
	if (spineDataCAN != NULL)
    {
        spineDataCAN->GetSensorData(sensors);

		robotgui->SetSensors(sensors,8);
		scene_->update();

        ui.lbl_andur1->setText(QString("Andur1: %1cm").arg(sensors[0]));
        ui.lbl_andur2->setText(QString("Andur2: %1cm").arg(sensors[1]));
        ui.lbl_andur3->setText(QString("Andur3: %1cm").arg(sensors[2]));
        ui.lbl_andur4->setText(QString("Andur4: %1cm").arg(sensors[3]));
		ui.lbl_andur5->setText(QString("Andur5: %1cm").arg(sensors[4]));
		ui.lbl_andur6->setText(QString("Andur6: %1cm").arg(sensors[5]));
		ui.lbl_andur7->setText(QString("Andur7: %1cm").arg(sensors[6]));
		ui.lbl_andur8->setText(QString("Andur8: %1cm").arg(sensors[7]));
    }
}
#endif

void FollowerUi::newUiData() 
{
    int sensors[8];

    dataLayer_->DL_getData(DLParamDistanceSensor1, sensors);
    dataLayer_->DL_getData(DLParamDistanceSensor2, sensors + 1);
    dataLayer_->DL_getData(DLParamDistanceSensor3, sensors + 2);
    dataLayer_->DL_getData(DLParamDistanceSensor4, sensors + 3);
    dataLayer_->DL_getData(DLParamDistanceSensor5, sensors + 4);
    dataLayer_->DL_getData(DLParamDistanceSensor6, sensors + 5);
    dataLayer_->DL_getData(DLParamDistanceSensor7, sensors + 6);
    dataLayer_->DL_getData(DLParamDistanceSensor8, sensors + 7);

	robotgui_->SetSensors(sensors, 8);
	scene_->update();

    ui.lbl_andur1->setText(QString("Andur1: %1cm").arg(sensors[0]));
    ui.lbl_andur2->setText(QString("Andur2: %1cm").arg(sensors[1]));
    ui.lbl_andur3->setText(QString("Andur3: %1cm").arg(sensors[2]));
    ui.lbl_andur4->setText(QString("Andur4: %1cm").arg(sensors[3]));
	ui.lbl_andur5->setText(QString("Andur5: %1cm").arg(sensors[4]));
	ui.lbl_andur6->setText(QString("Andur6: %1cm").arg(sensors[5]));
	ui.lbl_andur7->setText(QString("Andur7: %1cm").arg(sensors[6]));
	ui.lbl_andur8->setText(QString("Andur8: %1cm").arg(sensors[7]));  
}

void FollowerUi::keyPressEvent ( QKeyEvent * event )
 {
    if(workerThread_ == NULL) 
	{
		return;
	}

	int key = event->key();

	switch(key)
	{
	case Qt::Key_A:
		//KookKinematics::MotorsSpeedsFromAbsSpeed(ui.horizontalSlider_M1->value() / 10.0 , -M_PI/2, 0, speeds);
        qDebug() << "A";
        sendCmd(-1000, 1000, 0);
		break;
	case Qt::Key_D:
        //KookKinematics::MotorsSpeedsFromAbsSpeed(ui.horizontalSlider_M1->value() / 10.0, M_PI/2, 0, speeds);
		qDebug() << "D";
        sendCmd(1000, -1000, 0);
		break;
	case Qt::Key_W:
        //KookKinematics::MotorsSpeedsFromAbsSpeed(ui.horizontalSlider_M1->value() / 10.0, 0, 0, speeds);
		qDebug() << "W";
        sendCmd(1000, 1000, 0);
		break;
	case Qt::Key_Z:
        //KookKinematics::MotorsSpeedsFromAbsSpeed(ui.horizontalSlider_M1->value() / 10.0, M_PI, 0, speeds);
		sendCmd(-1000, -1000, 0);
        break;
    case Qt::Key_S:
        qDebug() << "S";
		sendCmd(0, 0, 0);
		break;
	}
 }

void FollowerUi::sendCmd( int w1, int  w2, int w3 )
{
    /*SpineCmdCAN* spineCmd = (SpineCmdCAN*)workerThread_->GetRemoteSpineCmd();
    spineCmd->SetWheelSpeeds(w1, w2, w3);
    workerThread_->SendSpineCmd(spineCmd);*/
    dataLayer_->DL_setData(DLParamMotor1RequestSpeed, &w1);
    dataLayer_->DL_setData(DLParamMotor2RequestSpeed, &w2);
    dataLayer_->DL_setData(DLParamMotor3RequestSpeed, &w3);
    emit newCommands();
}

void FollowerUi::wheelEvent (QWheelEvent* event)
{
	int steps = event->delta()/120;

	if (steps > 0){
		robotgui_->zoom *= 0.9;
	}else if (steps < 0){
		robotgui_->zoom *= 1.1;
	}
	 scene_->update();
}


void FollowerUi::mousePressEvent(QMouseEvent *event)
{
	
	
	if ((event->x() < ui.graphicsView->width()+12)&&(event->x()> 12)){
		if ((event->y() < ui.graphicsView->height()+12)&&(event->y()> 12)){
			robotgui_->MousePressEvent(event->x()-12-ui.graphicsView->width()/2,event->y()-12-ui.graphicsView->height()/2,event->buttons());
			scene_->update();
		}
	}
}
void FollowerUi::mouseMoveEvent(QMouseEvent *event)
{
//ui.lbl_andur1->setText(QString("move %1 %2").arg(event->x()).arg(event->y()));
}