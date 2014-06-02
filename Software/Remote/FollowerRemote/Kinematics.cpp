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

int Kinematics::getsensorerror()
{
	int errconst[4] = {-100,-10,10,-100};
	DLParam senslist[4] = {DLParamDistanceSensor8,DLParamDistanceSensor7,DLParamDistanceSensor1,DLParamDistanceSensor2};
	int sum = 0;
	unsigned char sensorvalue;


	for (int i = 0; i<4; i++)
	{
		dataLayer_->DL_getData(senslist[i], &sensorvalue);

		if ( sensorvalue <= 4 )
		{
			sensorvalue = 255;
		}

		sum += errconst[4]*(255-sensorvalue);
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

	//float mms_per_tick = 0.2633663366;


	/*if ((counter / 288) % 2)
	{
		motorspeed(1000,1000);
	}
	else
	{
		motorspeed(-1000,-1000);
	}
	*/
	
	

	//motorspeed(1000,1000);

	switch (last_state)
	{
		case 0:	speed = 1200; break;
		case 1:	speed -= 25; break;
		case 2:	speed += 25; break;
	}


	
	if ((sensorDiagLeft < 150)||(sensorDiagRight < 175)||(sensorLeft < 175)||(sensorRight < 150))
	{
		speed -= 25;
	}
	
	if ((sensorDiagLeft < 100)||(sensorDiagRight < 150)||(sensorLeft < 150)||(sensorRight < 100))
	{
		speed -= 50;
	}

	if ((sensorDiagLeft < 80)||(sensorDiagRight < 100)||(sensorLeft < 100)||(sensorRight < 80))
	{
		speed -= 75;
	}

    dataLayer_->DL_getData(DLParamDistanceSensor7, &sensorLeft);
    dataLayer_->DL_getData(DLParamDistanceSensor1, &sensorRight);

	if ( sensorLeft <= 4 ) 
		sensorLeft = 255;

	if ( sensorRight <= 4 ) 
		sensorRight = 255;

	dataLayer_->DL_getData(DLParamDistanceSensor8, &sensorDiagLeft);
	dataLayer_->DL_getData(DLParamDistanceSensor2, &sensorDiagRight);

	if ( sensorDiagLeft <= 4 ) 
		sensorDiagLeft = 255;

	if ( sensorDiagRight <= 4 ) 
		sensorDiagRight = 255;

	if ((sensorDiagLeft < 50)||(sensorDiagRight < 50)||(sensorLeft < 50)||(sensorRight < 50))
	{
		if (last_state)
		{
			counter++;
		}

		if (counter & 1)
		{
			motorspeed(600,-600);
		}
		else
		{
			motorspeed(-600,600);
		}

		last_state = 0;
		return;
	}

		if (speed < 1200)
		{
			speed = 1200;
		}

		if (speed > 5000)
		{
			speed = 5000;
		}

	if (sensorDiagLeft < sensorDiagRight)
	{

		if( sensorDiagLeft < 100)
		{
			motorspeed(speed,speed-int(speed/float(2+speed/1500)));
			last_state = 1;
		}
		else
		{
			motorspeed(speed,speed);
			last_state = 2;
		}
	}
	else
	{
		if( sensorDiagRight < 100)
		{
			last_state = 1;
			motorspeed(speed-int(speed/float(2+speed/1500)),speed);
		}
		else
		{
			last_state = 2;
			motorspeed(speed,speed);
		}
	}
	/*else if (sensorDiagLeft > 150)
	{
		motorspeed(1200,600);
	}
    else 
	{
		motorspeed(1200,1200);
    }
	*/



}