#include "Kinematics.h"
#include "DataLayerBase.h"
#include <qdebug.h>

Kinematics::Kinematics(DataLayerBase* dataLayer)
{
    state_ = Stop;
    currentM1_ = 0;
    currentM2_ = 0;
    requestM1_ = 0;
    requestM2_ = 0;
    dataLayer_ = dataLayer;
    connect(&timer_, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    running_ = false;
}

Kinematics::~Kinematics(void)
{
}


void Kinematics::startTimer()
{
    timer_.start(10);
}

void Kinematics::stopTimer()
{
    timer_.stop();
}

#if 0
void Kinematics::move(Direction direction, int speed)
{
    if (speed < 0)
        return;

    switch (direction)
    {
        case Forward:
            forward(speed);
            break;
        case Backward:
            break;
        case Left:
            break;
        case Right:
            break;
        case Stop:
            stop();
            break;
        default:
            break;
    }
    
}
#endif

void Kinematics::forward(int speed)
{
    state_ = Forward;
    requestM1_ = speed;
    requestM2_ = -speed;
    calculateAndSetSpeeds();
}

void Kinematics::backward(int speed)
{
    state_ = Backward;
    requestM1_ = -speed;
    requestM2_ = speed;
    calculateAndSetSpeeds();
}

void Kinematics::right(int speed)
{
    state_ = Right;
    requestM1_ = speed;
    requestM2_ = speed;
    calculateAndSetSpeeds();
}

void Kinematics::left(int speed)
{
    state_ = Left;
    requestM1_ = -speed;
    requestM2_ = -speed;
    calculateAndSetSpeeds();
}

void Kinematics::stop()
{
    //dataLayer_->DL_setData(DLParamMotor1RequestSpeed, 0);
    //dataLayer_->DL_setData(DLParamMotor2RequestSpeed, 0);
    state_ = Stop;
    requestM1_ = 0;
    requestM2_ = 0;
    calculateAndSetSpeeds();
}

void Kinematics::startAlgorithm()
{
    running_ = true;
    qDebug() << "Algorithm start";
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
        algorithm1();
    }
}


void Kinematics::timerUpdate()
{
    runAlgorithm();
    calculateAndSetSpeeds();  // smooth acceleration and stopping
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


void Kinematics::algorithm1()
{
    unsigned char sensorLeft;
    unsigned char sensorRight;

    dataLayer_->DL_getData(DLParamDistanceSensor1, &sensorLeft);
    dataLayer_->DL_getData(DLParamDistanceSensor5, &sensorRight);

    if( (sensorLeft < 30) || (sensorRight < 30))
    {
        stop();
        qDebug() << "stop";
    }
    else
    {
        forward(1200);
        qDebug() << "forward";
    }
}