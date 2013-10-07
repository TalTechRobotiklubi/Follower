#include <QKeyEvent>
#include "followerui.h"
#include "SpineCmdCAN.h"
#include "SpineDataCAN.h"
#include "TRobot.h"

FollowerUi::FollowerUi(Follower *robot)
	: QMainWindow()
{
	ui.setupUi(this); 
	workerThread_ = robot->workerThread;

    connect(workerThread_, SIGNAL(spineDataChanged(SpineData*)), this, SLOT(newUiData(SpineData*)));
    connect(ui.btnConnect, SIGNAL(clicked()), this, SLOT(connectSpine()));

	robotgui = new TRobot();
	scene = new QGraphicsScene(this);
	scene->setBackgroundBrush(QBrush(Qt::black));
	ui.graphicsView->setScene(scene);
	ui.graphicsView->setRenderHint(QPainter::Antialiasing);
	scene->addItem(robotgui);

}

FollowerUi::~FollowerUi()
{

	scene->clear();
	robotgui = NULL; 
	delete scene;

}

void FollowerUi::connectSpine() {

	if (workerThread_->isRunning()){
		workerThread_->Stop();
	}else{
		workerThread_->Start();
	}

	_sleep(1000);

	if (workerThread_->isRunning()){
		ui.btnConnect->setText("Disconnect");
	}else{
		ui.btnConnect->setText("Connect");
	}
}

void FollowerUi::newUiData(SpineData* spineData) {
    int sensors[8];

    SpineDataCAN* spineDataCAN = dynamic_cast<SpineDataCAN*>(spineData);
	
	if (spineDataCAN != NULL)
    {
        spineDataCAN->GetSensorData(sensors);

		robotgui->SetSensors(sensors,8);
		scene->update();

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

void FollowerUi::wheelEvent (QWheelEvent* event)
{
	int steps = event->delta()/120;

	if (steps > 0){
		robotgui->zoom *= 0.9;
	}else if (steps < 0){
		robotgui->zoom *= 1.1;
	}
	 scene->update();
}


void FollowerUi::mousePressEvent(QMouseEvent *event)
{
	
	
	if ((event->x() < ui.graphicsView->width()+12)&&(event->x()> 12)){
		if ((event->y() < ui.graphicsView->height()+12)&&(event->y()> 12)){
			robotgui->MousePressEvent(event->x()-12-ui.graphicsView->width()/2,event->y()-12-ui.graphicsView->height()/2,event->buttons());
			scene->update();
		}
	}
}
void FollowerUi::mouseMoveEvent(QMouseEvent *event)
{
//ui.lbl_andur1->setText(QString("move %1 %2").arg(event->x()).arg(event->y()));
}