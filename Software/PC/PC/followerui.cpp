#include "followerui.h"

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