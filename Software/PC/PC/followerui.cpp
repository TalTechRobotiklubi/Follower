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

	// added 16.07.2013 Arthur Randjärv ->
	robotgui = new TRobot();
	scene = new QGraphicsScene(this);
	scene->setBackgroundBrush(QBrush(Qt::black));
	ui.graphicsView->setScene(scene);
	ui.graphicsView->setRenderHint(QPainter::Antialiasing);
	scene->addItem(robotgui);
	//<- added 16.07.2013 Arthur Randjärv 
}

FollowerUi::~FollowerUi()
{
	// added 16.07.2013 Arthur Randjärv ->
	scene->clear(); // delete all objects on scene 
	robotgui = NULL; 
	delete scene;
	//<- added 16.07.2013 Arthur Randjärv
}

void FollowerUi::connectSpine() {
    workerThread_->Start();
}

void FollowerUi::newUiData(SpineData* spineData) {
    int sensors[6];

    SpineDataCAN* spineDataCAN = dynamic_cast<SpineDataCAN*>(spineData);
	
	if (spineDataCAN != NULL)
    {
        spineDataCAN->GetSensorData(sensors);

		robotgui->SetSensors(sensors,6); // added 16.07.2013 Arthur Randjärv 
		
        ui.lbl_andur1->setText(QString("Andur1: %1cm").arg(sensors[0]));
        ui.lbl_andur2->setText(QString("Andur2: %1cm").arg(sensors[1]));
        ui.lbl_andur3->setText(QString("Andur3: %1cm").arg(sensors[2]));
        ui.lbl_andur4->setText(QString("Andur4: %1cm").arg(sensors[3]));
		ui.lbl_andur5->setText(QString("Andur5: %1cm").arg(sensors[4]));
		ui.lbl_andur6->setText(QString("Andur6: %1cm").arg(sensors[5]));
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