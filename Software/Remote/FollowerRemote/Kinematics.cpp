#include "Kinematics.h"

#include <QDebug>

#include "DataLayerBase.h"
#include "algorithmbuilder.h"
#include "algorithmwander.h"

#include "windows.h"

Kinematics::Kinematics(DataLayerBase* dataLayer)
{
    currentM1_ = 0;
    currentM2_ = 0;
    requestM1_ = 0;
    requestM2_ = 0;
    dataLayer_ = dataLayer;
    connect(&timer_, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    running_ = false;
    algorithm_ = 0;

    currentCamX = 0;
    currentCamZ = 0;
    currentCamSpeedX = 0;
    currentCamspeedZ = 0;

    smoothControl = true;
    k_translationSpeed = 0;
    k_rotationSpeed = 0;
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

void Kinematics::forward(int speed)
{
  dataLayer_->DL_setData(DLParamRequestTranslationSpeed, &speed);
}

void Kinematics::backward(int speed)
{
  dataLayer_->DL_setData(DLParamRequestTranslationSpeed, &speed);
}

void Kinematics::right(int speed)
{
  dataLayer_->DL_setData(DLParamRequestRotationSpeed, &speed);
}

void Kinematics::left(int speed)
{
  dataLayer_->DL_setData(DLParamRequestRotationSpeed, &speed);
}

void Kinematics::setMotorSpeeds(int speedleft,int speedright)
{
    requestM1_ = speedleft;
    requestM2_ = -speedright;
    //calculateAndSetSpeeds();
}

void Kinematics::stop()
{
//    requestM1_ = 0;
//    requestM2_ = 0;
//    calculateAndSetSpeeds();
  int16_t speedX = 0;
  int16_t speedW = 0;
  dataLayer_->DL_setData(DLParamRequestTranslationSpeed, &speedX);
  dataLayer_->DL_setData(DLParamRequestRotationSpeed, &speedW);
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
    calculateAndSetSpeeds();  // smooth acceleration and stopping

}


void Kinematics::calculateAndSetSpeeds()
{


    if (smoothControl)
    {

        int key_up_down = GetKeyState(VK_UP) >> 1;
        int key_left_down = GetKeyState(VK_LEFT) >> 1;
        int key_right_down = GetKeyState(VK_RIGHT) >> 1;
        int key_down_down = GetKeyState(VK_DOWN) >> 1;

        int16_t angular_speed = 0;

        if (key_left_down)
            angular_speed = -k_rotationSpeed;

        if (key_right_down)
            angular_speed = k_rotationSpeed;

        if (key_right_down && key_left_down)
             angular_speed = 0;

        int16_t transSpeed = 0;

        if (key_up_down)
            transSpeed = k_translationSpeed;

        if (key_down_down)
            transSpeed = -k_translationSpeed;

        if (key_up_down && key_down_down)
             transSpeed = 0;

        // TODO (Arthur): dont update speed values if it is same than last value

        dataLayer_->DL_setData(DLParamRequestTranslationSpeed, &transSpeed);
        dataLayer_->DL_setData(DLParamRequestRotationSpeed, &angular_speed);
        //qDebug() << transSpeed << angular_speed;

    }

   /* currentM1_ = calculateNewSpeed(currentM1_, requestM1_);
    currentM2_ = calculateNewSpeed(currentM2_, requestM2_);
    dataLayer_->DL_setData(DLParamMotor1RequestSpeed, &currentM1_);
    dataLayer_->DL_setData(DLParamMotor2RequestSpeed, &currentM2_);
   */

    //qDebug() << currentM1_ << currentM2_;

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

int Kinematics::calculateNewSpeed(int currentSpeed, int requestedSpeed)
{
    if (currentSpeed > requestedSpeed)
    {
        if (abs(currentSpeed - requestedSpeed) >= ACCELER_RATE)
        {
            currentSpeed = currentSpeed - ACCELER_RATE;
        }
        else
        {
            currentSpeed = requestedSpeed;
        }
    }
    else if (requestedSpeed > currentSpeed)
    {
        if (abs(requestedSpeed - currentSpeed) >= ACCELER_RATE)
        {
            currentSpeed = currentSpeed + ACCELER_RATE;
        }
        else
        {
            currentSpeed = requestedSpeed;
        }
    }
    else
    {
        // no change as currentSpeed == requestedSpeed
    }
    return currentSpeed;
}

void Kinematics::updateFromUi(int translationSpeed,int rotationSpeed, bool smcon)
{
    smoothControl = smcon;
    k_translationSpeed = translationSpeed;
    k_rotationSpeed = rotationSpeed;
}
