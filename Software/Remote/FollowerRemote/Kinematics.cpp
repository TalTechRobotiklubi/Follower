#include "Kinematics.h"

#include <QDebug>

#include "DataLayerBase.h"
#include "algorithmbuilder.h"
#include "algorithmwander.h"

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
    requestM1_ = speed;
    requestM2_ = -speed;
    calculateAndSetSpeeds();
}

void Kinematics::backward(int speed)
{
    requestM1_ = -speed;
    requestM2_ = speed;
    calculateAndSetSpeeds();
}

void Kinematics::right(int speed)
{
    requestM1_ = speed/2;
    requestM2_ = speed/2;
    calculateAndSetSpeeds();
}

void Kinematics::left(int speed)
{
    requestM1_ = -speed/2;
    requestM2_ = -speed/2;
    calculateAndSetSpeeds();
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
    //calculateAndSetSpeeds();  // smooth acceleration and stopping
}


void Kinematics::calculateAndSetSpeeds()
{
    currentM1_ = calculateNewSpeed(currentM1_, requestM1_);
    currentM2_ = calculateNewSpeed(currentM2_, requestM2_);
    dataLayer_->DL_setData(DLParamMotor1RequestSpeed, &currentM1_);
    dataLayer_->DL_setData(DLParamMotor2RequestSpeed, &currentM2_);
    //qDebug() << currentM1_ << currentM2_;
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
