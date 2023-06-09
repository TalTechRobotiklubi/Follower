#include "followerui.h"

#include <QKeyEvent>
#include <QtSerialPort/QSerialPortInfo>
#include <QList>
#include <qdebug.h>

#define _USE_MATH_DEFINES 1

#include <math.h>

#include "WorkerThreadBase.h"
#include "WorkerObjectBase.h"
#include "DataLayerBase.h"
#include "Kinematics.h"
#include "TRobot.h"
#include "configure.h"
#include "chartdisplay.h"

FollowerUi::FollowerUi(Follower *robot)
  : QMainWindow()
{
  ui.setupUi(this);
  settings_ = robot->getSettings();
  workerThread_ = robot->getWorkerThread();
  workerObject_ = robot->getWorkerObject();
  dataLayer_ = robot->getWorkerObject()->getDataLayer();
  kinematics_ = robot->getKinematics();
  keyboardOff_ = false;

  workerObject_->moveToThread(workerThread_);
  connect(this, SIGNAL(startCommunication(QString)), workerObject_, SLOT(startCommunication(QString)));
  connect(workerObject_, SIGNAL(startStatus(bool)), this, SLOT(startCommStatus(bool)));
  connect(workerObject_, SIGNAL(newDataReceived()), this, SLOT(newUiData()));
  connect(this, SIGNAL(stopCommunication()), workerObject_, SLOT(stopCommunication()));
  connect(workerObject_, SIGNAL(stopStatus()), this, SLOT(stopCommStatus()));
  connect(ui.btnConnect, SIGNAL(clicked()), this, SLOT(connectSpine()));
  connect(ui.BtnSeartchPorts, SIGNAL(clicked()), this, SLOT(updatePortList()));

  // Start the workerThread_ here.
  // Do not close until program exit or emergency.
  workerThread_->start();

  robotgui_ = new TRobot(settings_);
  scene_ = new QGraphicsScene(this);
  scene_->setBackgroundBrush(QBrush(Qt::black));
  ui.graphicsView->setScene(scene_);
  ui.graphicsView->setRenderHint(QPainter::Antialiasing);
  scene_->addItem(robotgui_);

  setupPidConfig();

  qDebug() << "Liikumine - WASD";
  qDebug() << "Algoritm start - 1,2,3";
  qDebug() << "Algoritm stop - P";
  qDebug() << "Kiirus juurde - I";
  qDebug() << "Kiirus maha - O";

  updatePortList();
}

FollowerUi::~FollowerUi()
{

  scene_->clear();
  robotgui_ = nullptr;
  delete scene_;
  delete conf_;

}

void FollowerUi::connectSpine() {

  if (!workerThread_->isRunning())
  {
    qDebug() << "Fatal error: workerThread_ is not runnning!";
    return;
  }

  /*
   * Note: behaviour is flipped here due to the button's checked value being
   * changed /before/ this function is called. So the below only applies here.
   * ui.btnConnect->isChecked() -> True => communications are not running
   *                            -> False => communications are running
   */
  if (ui.btnConnect->isChecked())
  {
    emit startCommunication(ui.CBPort->currentText());
  }
  else
  {
    // open communication
    emit stopCommunication();
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
    ui.btnConnect->setText(QString("Disconnect"));
    // start timer in kinematics
    kinematics_->startTimer();
  }
}

void FollowerUi::stopCommStatus()
{
  // stop timer in kinematics
  kinematics_->stopTimer();
  // set feedback to user that disconnection was ok
  ui.btnConnect->setText(QString("Connect"));
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

  int16_t qw = 0;
  int16_t qx = 0;
  int16_t qy = 0;
  int16_t qz = 0;
  dataLayer_->DL_getData(DLParamQw, &qw);
  dataLayer_->DL_getData(DLParamQx, &qx);
  dataLayer_->DL_getData(DLParamQy, &qy);
  dataLayer_->DL_getData(DLParamQz, &qz);

  calcAndWriteEulerAnglesToUI(qw, qx, qy, qz);

  QList<uint8_t> data = {0,0,0,0};
  bool newData = dataLayer_->DL_getData(DLParamRobotFeedback1, &data[0]);
  newData |= dataLayer_->DL_getData(DLParamRobotFeedback2, &data[1]);
  newData |= dataLayer_->DL_getData(DLParamRobotFeedback3, &data[2]);
  newData |= dataLayer_->DL_getData(DLParamRobotFeedback4, &data[3]);
  if (newData)
    emit feedbackReceived(data);
}

void FollowerUi::keyPressEvent(QKeyEvent * event)
{
  if(!workerThread_ || event->isAutoRepeat() || keyboardOff_)
    return;

  int key = event->key();
  int tSpeed = ui.sb_setSpeed->value();
  int wSpeed = ui.sb_setAngularSpeed->value();

  switch(key)
  {
    case Qt::Key_Left:
      kinematics_->setSpeeds(tSpeed, -wSpeed);
      break;
    case Qt::Key_Right:
      kinematics_->setSpeeds(tSpeed, wSpeed);
      break;
    case Qt::Key_Up:
      kinematics_->setSpeeds(tSpeed, wSpeed);
      break;
    case Qt::Key_Down:
      kinematics_->setSpeeds(-tSpeed, wSpeed);
      break;
    case Qt::Key_S:
      kinematics_->stop();
      break;
    case Qt::Key_I:
      ui.sb_setSpeed->setValue(tSpeed + 100);
      break;
    case Qt::Key_O:
      ui.sb_setSpeed->setValue(tSpeed - 100);
      break;
    case Qt::Key_1:  // run
      kinematics_->startAlgorithm(1);
      break;
    case Qt::Key_2:  // run
      kinematics_->startAlgorithm(2);
      break;
    case Qt::Key_3:  // run
      kinematics_->startAlgorithm(3);
      break;
    case Qt::Key_P:  // stop
      kinematics_->stopAlgorithm();
      break;
    case Qt::Key_U:  // camera up
      kinematics_->cameraLook(0,+1);
      break;
    case Qt::Key_J: // camera down
      kinematics_->cameraLook(0,-1);
      break;
    case Qt::Key_H:
      kinematics_->cameraLook(+1,0);
      break;
    case Qt::Key_K:
      kinematics_->cameraLook(-1,0);
      break;
  }
}

void FollowerUi::keyReleaseEvent(QKeyEvent* event)
{
  if(!workerThread_ || event->isAutoRepeat() || keyboardOff_)
    return;

  int key = event->key();
  switch(key)
  {
    case Qt::Key_Left:
      kinematics_->setSpeeds(kinematics_->translationSpeed(), 0);
      break;
    case Qt::Key_Right:
      kinematics_->setSpeeds(kinematics_->translationSpeed(), 0);
      break;
    case Qt::Key_Up:
      kinematics_->setSpeeds(0, kinematics_->rotationSpeed());
      break;
    case Qt::Key_Down:
      kinematics_->setSpeeds(0, kinematics_->rotationSpeed());
      break;
    default:
      break;
  }
}

void FollowerUi::wheelEvent (QWheelEvent* event)
{
  int steps = event->delta()/120;

  if (steps > 0)
    robotgui_->zoom *= 0.9;
  else if (steps < 0)
    robotgui_->zoom *= 1.1;
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

void FollowerUi::mouseMoveEvent(QMouseEvent*)
{
  //ui.lbl_andur1->setText(QString("move %1 %2").arg(event->x()).arg(event->y()));
}

void FollowerUi::closeEvent(QCloseEvent* )
{
  if (ui.btnConnect->isChecked())
  {
    emit stopCommunication();
  }
}

void FollowerUi::updatePortList()
{

  QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
  QSerialPortInfo port;
  int i;

  ui.CBPort->clear();

  if (list.count() > 0)
  {
    for (i = 0; i < list.count();i++)
    {
      port = list[i];
      ui.CBPort->addItem(port.portName());
    }
  }
}

void FollowerUi::keyboardDisabled(bool disabled)
{
  keyboardOff_ = disabled;
}

void FollowerUi::calcAndWriteEulerAnglesToUI(int16_t raw_qw, int16_t raw_qx,
                                             int16_t raw_qy, int16_t raw_qz)
{
  //qDebug() << "Quaternions: " << raw_qw << raw_qx << raw_qy << raw_qz;

  double qw = raw_qw / 16384.0;
  double qx = raw_qx / 16384.0;
  double qy = raw_qy / 16384.0;
  double qz = raw_qz / 16384.0;

  double qww = qw*qw;
  double qxx = qx*qx;
  double qyy = qy*qy;
  double qzz = qz*qz;

  double yaw = atan2(2.0f * (qx * qy + qw * qz), qww + qxx -qyy - qzz);
  double pitch = -asin(2.0f * (qx* qz - qw * qy));
  double roll = atan2(2.0f * (qw * qx + qy * qz), qww - qxx - qyy + qzz);

  yaw *= 180.0f / M_PI;
  pitch *= 180.0f / M_PI;
  roll *= 180.0f / M_PI;

  ui.lbl_yaw->setText(QString("Yaw: %1").arg(yaw));
  ui.lbl_pitch->setText(QString("Pitch: %1").arg(pitch));
  ui.lbl_roll->setText(QString("Roll: %1").arg(roll));
}

// Sets up the conf_ variable.
// Called at object construction.
void FollowerUi::setupPidConfig()
{
  conf_ = new Configure(settings_, kinematics_);
  connect(conf_, &Configure::sendParameter,
          [=](uint8_t param, float value){
    uint8_t update = 0;
    this->dataLayer_->DL_setData(DLParamPidParameter, &param);
    this->dataLayer_->DL_setData(DLParamPidValue, &value);
    this->dataLayer_->DL_setData(DLParamPidUpdating, &update);
  });
  connect(conf_, &Configure::parameterSendFinished,
          [=]() {
    uint8_t update = 1;
    this->dataLayer_->DL_setData(DLParamPidUpdating, &update);
  });
  connect(this, &FollowerUi::feedbackReceived, conf_, &Configure::onNewFeedbackData);
  connect(conf_, &Configure::routineStatus, this, &FollowerUi::keyboardDisabled);

  ui.tabWidget->addTab(conf_, "PID Config");

  charts_ = new ChartDisplay();
  ui.tabWidget->addTab(charts_, "Data Charts");
}
