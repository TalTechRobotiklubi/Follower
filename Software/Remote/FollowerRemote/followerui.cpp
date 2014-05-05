#include <QKeyEvent>
#include "followerui.h"
#include "WorkerThreadBase.h"
#include "WorkerObjectBase.h"
#include "DataLayerBase.h"
#include "Kinematics.h"
#include "TRobot.h"
#include <qdebug.h>


FollowerUi::FollowerUi(Follower *robot)
	: QMainWindow()
{
	ui.setupUi(this); 
	workerThread_ = robot->getWorkerThread();
    workerObject_ = robot->getWorkerObject();
    dataLayer_ = robot->getWorkerObject()->getDataLayer();
    kinematics_ = robot->getKinematics();

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
        emit stopCommunication();
	}
    else
    {
		workerThread_->start();
        // open communication
        emit startCommunication();
	}
}

void FollowerUi::startCommStatus(bool status)
{
    //connection failed. Stop thread.
    if (!status)
    {
        workerThread_->exit();
        workerThread_->wait();
    }
    else
    {
        // set feedback to user that connection was ok
        ui.btnConnect->setText("Disconnect");
        // start timer in kinematics
        kinematics_->startTimer();
    }

}

void FollowerUi::stopCommStatus(bool status)
{
    // stop timer in kinematics
    kinematics_->stopTimer();
    //stop thread anyway.
    workerThread_->exit();
    workerThread_->wait();
    // set feedback to user that disconnection was ok
    ui.btnConnect->setText("Connect");
}


void FollowerUi::newUiData() 
{
    unsigned char sensors[8];

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

    // speed is int16 value
    int16_t speed1 = 0;
    int16_t speed2 = 0;
    dataLayer_->DL_getData(DLParamMotor1ActualSpeed, &speed1);
    dataLayer_->DL_getData(DLParamMotor2ActualSpeed, &speed2);
	robotgui_->m1speed  = speed1;
	robotgui_->m2speed  = -speed2;


    ui.lbl_leftSpeed->setText(QString("Vasak: %1m/s").arg(speed1));
    ui.lbl_rightSpeed->setText(QString("Parem: %1m/s").arg(speed2));

    int8_t accX = 0;
    int8_t accY = 0;
    int8_t accZ = 0;

    dataLayer_->DL_getData(DLParamAccelerationX, &accX);
    dataLayer_->DL_getData(DLParamAccelerationY, &accY);
    dataLayer_->DL_getData(DLParamAccelerationZ, &accZ);

    ui.lbl_accX->setText(QString("AccX: %1").arg(accX));
    ui.lbl_accY->setText(QString("AccY: %1").arg(accY));
    ui.lbl_accZ->setText(QString("AccZ: %1").arg(accZ));

    //qDebug() << accX << accY << accZ;
}

void FollowerUi::keyPressEvent ( QKeyEvent * event )
 {
    if(workerThread_ == NULL) 
	{
		return;
	}

	int key = event->key();
    int setSpeed = ui.sb_setSpeed->value();

	switch(key)
	{
	case Qt::Key_A:
        //sendCmd(-setSpeed, -setSpeed, 0);
        kinematics_->left(setSpeed);
		break;
	case Qt::Key_D:
        //sendCmd(setSpeed, setSpeed, 0);
        kinematics_->right(setSpeed);
		break;
	case Qt::Key_W:
        //sendCmd(-setSpeed, setSpeed, 0);
        kinematics_->forward(setSpeed);
		break;
	case Qt::Key_Z:
		//sendCmd(setSpeed, -setSpeed, 0);
        kinematics_->backward(setSpeed);
        break;
    case Qt::Key_S:
		//sendCmd(0, 0, 0);
        kinematics_->stop();
		break;
    case Qt::Key_I:
        ui.sb_setSpeed->setValue(setSpeed + 100);
        break;
    case Qt::Key_O:
        ui.sb_setSpeed->setValue(setSpeed - 100);
        break;
    case Qt::Key_R:  // run
        kinematics_->startAlgorithm();
        break;
    case Qt::Key_P:  // stop
        kinematics_->stopAlgorithm();
        break;
	}
 }

void FollowerUi::sendCmd( int w1, int  w2, int w3 )
{
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
	if ((event->x() < ui.graphicsView->width()+12)&&(event->x()> 12))
    {
		if ((event->y() < ui.graphicsView->height()+12)&&(event->y()> 12))
        {
			robotgui_->MousePressEvent(event->x()-12-ui.graphicsView->width()/2,event->y()-12-ui.graphicsView->height()/2,event->buttons());
			scene_->update();
		}
	}
}
void FollowerUi::mouseMoveEvent(QMouseEvent *event)
{
//ui.lbl_andur1->setText(QString("move %1 %2").arg(event->x()).arg(event->y()));
}