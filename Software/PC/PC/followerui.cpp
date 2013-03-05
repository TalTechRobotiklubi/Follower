#include "followerui.h"

FollowerUi::FollowerUi(Follower *robot)
	: QMainWindow()
{
	ui.setupUi(this);
    workerThread_ = robot->workerThread;

    connect(ui.btnConnect, SIGNAL(clicked()), this, SLOT(connectSpine()));
}

FollowerUi::~FollowerUi()
{

}

void FollowerUi::connectSpine() {
    workerThread_->Start();
}

void FollowerUi::newSensorData(int s1, int s2, int s3, int s4) {
    ui.lbl_andur1->setText(QString(s1));
    ui.lbl_andur1->setText(QString(s2));
    ui.lbl_andur1->setText(QString(s3));
    ui.lbl_andur1->setText(QString(s4));
}