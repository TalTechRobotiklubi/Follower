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
	currentAlgorithm = 1;
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
    requestM1_ = speed/2;
    requestM2_ = speed/2;
    calculateAndSetSpeeds();
}

void Kinematics::left(int speed)
{
    state_ = Left;
    requestM1_ = -speed/2;
    requestM2_ = -speed/2;
    calculateAndSetSpeeds();
}

void Kinematics::motorspeed(int speedleft,int speedright)
{
    state_ = Forward; // TODO: forward is not good


    requestM1_ = speedleft;
    requestM2_ = -speedright;

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

void Kinematics::startAlgorithm(int algorithmNum)
{
    running_ = true;
	currentAlgorithm = algorithmNum;
    qDebug() << "Algorithm"<< currentAlgorithm <<" start";
}

void Kinematics::stopAlgorithm()
{   
    running_ = false;
    stop();
    qDebug() << "Algorithm"<< currentAlgorithm <<" stop";
}

void Kinematics::runAlgorithm()
{
    if (running_)
    {
		switch (currentAlgorithm)
		{
			case 1: algorithm1(); break;
			case 2: algorithm2(); break;
		}
			
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

int Kinematics::getsensorerror()
{
	int errconst[4] = {-50,10,10,50};
	DLParam senslist[4] = {DLParamDistanceSensor8,DLParamDistanceSensor7,DLParamDistanceSensor1,DLParamDistanceSensor2};
	int sum = 0;
	unsigned char sensorvalue;


	for (int i = 0; i<4; i++)
	{
		dataLayer_->DL_getData(senslist[i], &sensorvalue);

		if ( sensorvalue == 0 )
		{
			sensorvalue = 255;
		}


		sum += errconst[i]*(255-sensorvalue);
	}

	return sum;
}

void Kinematics::algorithm1()
{
    unsigned char sensorLeft;
    unsigned char sensorRight;

	unsigned char sensorDiagLeft;
    unsigned char sensorDiagRight;

	static int counter = 0;
	static int last_state = 0;
	static int speed = 1200;

	int stop_distance = 50;


	static int p,i,d ;

    dataLayer_->DL_getData(DLParamDistanceSensor7, &sensorLeft);
    dataLayer_->DL_getData(DLParamDistanceSensor1, &sensorRight);
	dataLayer_->DL_getData(DLParamDistanceSensor8, &sensorDiagLeft);
	dataLayer_->DL_getData(DLParamDistanceSensor2, &sensorDiagRight);



	if (((sensorDiagLeft <stop_distance)&&(sensorDiagLeft))
		||((sensorDiagRight < stop_distance)&&(sensorDiagRight))
		||((sensorLeft < stop_distance)&&(sensorLeft))
		||((sensorRight < stop_distance)&&(sensorRight)))
	{

		if (last_state)
		{
			counter++;
		}
			if (counter & 1)
			{
				motorspeed(900,-900);
			}
			else
			{
				motorspeed(-900,900);
			}


		last_state = 0;
		i = 0;
		d = 0;
		p = 0;
		return;
	}


	d = p;

	p = getsensorerror()/25;
	
	if (p > 1000)
	{
		p = 1000;
	}

	if (p < -1000)
	{
		p = -1000;
	}
	d = p-d;

	speed = 1200;//2000-abs(p);

	if ((i < 50000)&&(i >-50000))
	{
		i += p;
	}

	int pid = p;//+i/75+d/4;

	motorspeed(speed-pid,speed+pid);
	last_state = 1;

}

void Kinematics::algorithm2()
{

	uint16_t TrackedObjectX;
	uint16_t TrackedObjectY;
	uint16_t TrackedObjectHeight;
	uint16_t TrackedObjectWidth;

	dataLayer_->DL_getData(DLParamTrackedObjectX, &TrackedObjectX);
	dataLayer_->DL_getData(DLParamTrackedObjectY, &TrackedObjectY);
	dataLayer_->DL_getData(DLParamTrackedObjectHeight, &TrackedObjectHeight);
	dataLayer_->DL_getData(DLParamTrackedObjectWidth, &TrackedObjectWidth);

	qDebug() << "TrackedObject X Y H W"<< TrackedObjectX << " "<< TrackedObjectY <<" "<< TrackedObjectHeight <<" "<< TrackedObjectWidth ;
}