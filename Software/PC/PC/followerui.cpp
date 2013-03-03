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