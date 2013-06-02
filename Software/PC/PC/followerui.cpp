#include <QKeyEvent>
#include "followerui.h"
#include "SpineCmdCAN.h"

FollowerUi::FollowerUi(Follower *robot)
	: QMainWindow()
{
	ui.setupUi(this);
    workerThread_ = robot->workerThread;

    connect(workerThread_, SIGNAL(newSensorData()), this, SLOT(newSensorData()));
    connect(ui.btnConnect, SIGNAL(clicked()), this, SLOT(connectSpine()));
}

FollowerUi::~FollowerUi()
{

}

void FollowerUi::connectSpine() {
    workerThread_->Start();
}

void FollowerUi::newUiData(SpineDataCAN* spineData) {
    int sensors[6];

    spineData->GetSensorData(sensors);

    ui.lbl_andur1->setText(QString(sensors[0]));
    ui.lbl_andur1->setText(QString(sensors[1]));
    ui.lbl_andur1->setText(QString(sensors[2]));
    ui.lbl_andur1->setText(QString(sensors[3]));
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
        sendCmd(-100, 100, 0);
		break;
	case Qt::Key_D:
        //KookKinematics::MotorsSpeedsFromAbsSpeed(ui.horizontalSlider_M1->value() / 10.0, M_PI/2, 0, speeds);
		sendCmd(100, -100, 0);
		break;
	case Qt::Key_W:
        //KookKinematics::MotorsSpeedsFromAbsSpeed(ui.horizontalSlider_M1->value() / 10.0, 0, 0, speeds);
		sendCmd(100, 100, 0);
		break;
	case Qt::Key_Z:
        //KookKinematics::MotorsSpeedsFromAbsSpeed(ui.horizontalSlider_M1->value() / 10.0, M_PI, 0, speeds);
		sendCmd(-100, -100, 0);
        break;
    case Qt::Key_S:
		sendCmd(0, 0, 0);
		break;
	}
 }

void FollowerUi::sendCmd( int w1, int  w2, int w3 )
{
    SpineCmdCAN* spineCmd = (SpineCmdCAN*)workerThread_->GetRemoteSpineCmd();
    spineCmd->SetWheelSpeeds(w1, w2, w3);
    workerThread_->SendSpineCmd(spineCmd);
}