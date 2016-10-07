#include "Kinematics.h"

#include <QDebug>

#include "DataLayerBase.h"
#include "algorithmbuilder.h"
#include "algorithmwander.h"

#include "windows.h"

Kinematics::Kinematics(DataLayerBase* dataLayer)
{
    dataLayer_ = dataLayer;
    connect(&timer_, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    running_ = false;
    algorithm_ = 0;

    currentCamX = 0;
    currentCamZ = 0;
    currentCamSpeedX = 0;
    currentCamspeedZ = 0;

    translationSpeed_ = 0;
    rotationSpeed_ = 0;
}

Kinematics::~Kinematics(void)
{
    delete algorithm_;
}


void Kinematics::startTimer()
{
    timer_.start(10);
}

void Kinematics::stopTimer()
{
    timer_.stop();
}

int Kinematics::translationSpeed()
{
  return translationSpeed_;
}

int Kinematics::rotationSpeed()
{
  return rotationSpeed_;
}

//void Kinematics::setMotorSpeeds(int speedleft,int speedright)
//{
//    requestM1_ = speedleft;
//    requestM2_ = -speedright;
//    //calculateAndSetSpeeds();
//}

void Kinematics::stop()
{
  translationSpeed_ = 0;
  rotationSpeed_ = 0;
}

void Kinematics::startAlgorithm(int algorithmNum)
{
    running_ = true;
    delete algorithm_;
    algorithm_ = AlgorithmBuilder::create(algorithmNum, dataLayer_);
    qDebug() << "Algorithm"<< algorithmNum <<" start";
}

void Kinematics::stopAlgorithm()
{   
    running_ = false;
    stop();
    qDebug() << "Algorithm stop";
}

void Kinematics::cameraLook(int x, int z)
{
  currentCamSpeedX = x;
  currentCamspeedZ = z;
}

void Kinematics::setSpeeds(int translationSpeed, int rotationSpeed)
{
    translationSpeed_ = translationSpeed;
    rotationSpeed_ = rotationSpeed;
}

void Kinematics::runAlgorithm()
{
    if (running_)
    {
      IAlgorithm::Execution exec = algorithm_->run();
      dataLayer_->DL_setData(DLParamRequestTranslationSpeed, &exec.xSpeed);
      dataLayer_->DL_setData(DLParamRequestRotationSpeed, &exec.wSpeed);
    }
}

void Kinematics::timerUpdate()
{
    runAlgorithm();
    calculateAndSendSpeeds();
}

void Kinematics::calculateAndSendSpeeds()
{
    dataLayer_->DL_setData(DLParamRequestTranslationSpeed, &translationSpeed_);
    dataLayer_->DL_setData(DLParamRequestRotationSpeed, &rotationSpeed_);

    currentCamX += currentCamSpeedX;
    currentCamZ += currentCamspeedZ;

    if (currentCamX > 100)
    {
     currentCamX = 100;
    }
    else if (currentCamX < -100)
    {
     currentCamX = -100;
    }

    if (currentCamZ > 100)
    {
     currentCamZ = 100;
    }
    else if (currentCamZ < -100)
    {
     currentCamZ = -100;
    }

    dataLayer_->DL_setData(DLParamCameraRequestXDegree, &currentCamX);
    dataLayer_->DL_setData(DLParamCameraRequestZDegree, &currentCamZ);

}
