#include "Drive.h"

#include "math.h"

#include "DataLayer.h"

typedef enum
{
	KP_X = 0,
	KI_X = 1,
	KD_X = 2,
	KP_W = 3,
	KI_W = 4,
	KD_W = 5,
	ACC_X = 6,
	ACC_W = 7
} PidParam;

typedef struct
{
	double yaw;
	double pitch;
	double roll;
} Euler;

static float kpX = 0.0f;
static float kiX = 0.0f;
static float kdX = 0.0f;
static float kpW = 0.0f;
static float kiW = 0.0f;
static float kdW = 0.0f;
static float accX_c = 20.0f;
static float accW_c = 0.4f;
static Euler position = {0.0, 0.0, 0.0};
static int32_t leftEncoder = 0;  //are 32-bits enough
static int32_t rightEncoder = 0;
static float speedX = 0;
static float speedW = 0;

static uint8_t updateParameters();
static void calculateEuler();
static void readEncoders();
static void stop();
static void readRequestedSpeeds();
static void drive();

uint8_t updateParameters()
{
	uint8_t isUpdating;
	DL_getData(DLParamPidUpdating, &isUpdating);
	if (isUpdating)
	{
		float value;
		PidParam param;
		if (DL_getData(DLParamPidValue, &value) &&
				DL_getData(DLParamPidParameter, &param))
		{
			switch(param)
			{
			case KP_X:
				kpX = value;
				break;
			case KI_X:
				kiX = value;
				break;
			case KD_X:
				kdX = value;
				break;
			case KP_W:
				kpW = value;
				break;
			case KI_W:
				kiW = value;
				break;
			case KD_W:
				kdW = value;
				break;
			case ACC_X:
				accX_c = value;
				break;
			case ACC_W:
				accW_c = value;
				break;
			default:
				break;
			}
		}
		return 1;
	}
	return 0;
}

void calculateEuler()
{
	int16_t read;
	double qw;
	double qx;
	double qy;
	double qz;
	DL_getData(DLParamQw, &read);
	qw = read / 16384;
	DL_getData(DLParamQx, &read);
	qx = read / 16384;
	DL_getData(DLParamQy, &read);
	qy = read / 16384;
	DL_getData(DLParamQz, &read);
	qz = read / 16384;

	int32_t qww = qw*qw;
	int32_t qxx = qx*qx;
	int32_t qyy = qy*qy;
	int32_t qzz = qz*qz;

	position.yaw = atan2(2.0f * (qx * qy + qw * qz), qww + qxx -qyy - qzz);
	position.pitch = -asin(2.0f * (qx* qz - qw * qy));
	position.roll = atan2(2.0f * (qw * qx + qy * qz), qww - qxx - qyy + qzz);

	position.yaw *= 180.0f / M_PI;
	position.pitch *= 180.0f / M_PI;
	position.roll *= 180.0f / M_PI;
}

void readEncoders()
{
	int16_t left;
	int16_t right;
	if (DL_getData(DLParamMotor1EncoderClicks, &left)) // ??? motor 1is leftor right
		leftEncoder += left;
	if (DL_getData(DLParamMotor2EncoderClicks, &right))
		rightEncoder += right;
}

void stop()
{
	int16_t left = 0;
	int16_t right = 0;

	left = 0;
	right = 0;
	DL_setData(DLParamMotor1RequestSpeed, &left);
	DL_setData(DLParamMotor2RequestSpeed, &right);
}

void readRequestedSpeeds()
{
	int16_t read;
	DL_getData(DLParamRequestTranslationSpeed, &read);
	speedX = (float)read;
	DL_getData(DLParamRequestRotationSpeed, &read);
	speedW = (float)read;
}

void drive()
{
	static float fwd_speed = 0, turn_speed = 0;
	static int16_t rightSpeedOld = 0, leftSpeedOld = 0;
	static float posXold = 0, posWold = 0;
	float posX = 0, posW = 0;
	float errX, errW;
	int encX, encW;
	int rightSpeed = 0, leftSpeed = 0;

	// Linear acceleration
	if(fwd_speed < speedX)
		fwd_speed += accX_c;
	else if(fwd_speed > speedX)
		fwd_speed -= accX_c;

	// Rotational acceleration
	if(turn_speed < speedW)
		turn_speed += accW_c;
	else if(turn_speed > speedW)
		turn_speed -= accW_c;

	// calculate linear and angular speed
	encX = (rightEncoder + leftEncoder)/2;
	encW = (rightEncoder - leftEncoder);

	// calculate linear and angular speed errors
	posX = fwd_speed - encX;
	posW = turn_speed - encW;

	// Calculate driving errors for PID
	errX = kpX * posX + kdX * (posX - posXold);
	errW = kpW * posW + kdW * (posW - posWold);

	rightSpeed = errX + errW;
	leftSpeed = errX - errW;

	//  Calculate new speed values
	rightSpeedOld += rightSpeed;
	leftSpeedOld += leftSpeed;

	if(!speedX && !speedW)
	{
		rightSpeedOld = 0;
		leftSpeedOld = 0;
	}

	// Set motor speeds
	DL_setData(DLParamMotor1RequestSpeed, &leftSpeedOld);
	DL_setData(DLParamMotor2RequestSpeed, &rightSpeedOld);

	// Save old values for PID
	posXold = posX;
	posWold = posW;

	leftEncoder = 0;
	rightEncoder = 0;
}

void Drive_init()
{
	uint8_t isUpdating = 0;
	DL_setData(DLParamPidUpdating, &isUpdating);
}

void Drive_task()
{
	if (updateParameters())
	{
		stop();
		return;
	}
	calculateEuler();
	readEncoders();
	readRequestedSpeeds();
	drive();
}

void Drive_setSpeed(float speedX, float speedW)
{

}

