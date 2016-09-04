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
	float yaw;
	float pitch;
	float roll;
} Euler;

static float kpX = 0.0f;
static float kiX = 0.0f;
static float kdX = 0.0f;
static float kpW = 0.0f;
static float kiW = 0.0f;
static float kdW = 0.0f;
static float accX = 20.0f;
static float accW = 0.4f;

static float kpX_t = 0.0f;  // temporary fields
static float kiX_t = 0.0f;
static float kdX_t = 0.0f;
static float kpW_t = 0.0f;
static float kiW_t = 0.0f;
static float kdW_t = 0.0f;
static float accX_t = 0.0f;
static float accW_t = 0.0f;

static int32_t leftEncoder = 0;  //are 32-bits enough
static int32_t rightEncoder = 0;
static float speedX = 0;
static float speedW = 0;

static uint8_t updateParameters();
static void readEncoders();
static void stop();
static void readRequestedSpeeds();
static void readTurningSpeeds();
static void drive();

uint8_t updateParameters()
{
	float value;
	PidParam param;
	uint8_t isUpdating;
	Bool result = DL_getData(DLParamPidValue, &value);
	result = result && DL_getData(DLParamPidParameter, &param);
	if (result)
	{
		switch(param)
		{
		case KP_X:
			kpX_t = value;
			break;
		case KI_X:
			kiX_t = value;
			break;
		case KD_X:
			kdX_t = value;
			break;
		case KP_W:
			kpW_t = value;
			break;
		case KI_W:
			kiW_t = value;
			break;
		case KD_W:
			kdW_t = value;
			break;
		case ACC_X:
			accX_t = value;
			break;
		case ACC_W:
			accW_t = value;
			break;
		default:
			break;
		}
	}

	if (DL_getData(DLParamPidUpdating, &isUpdating))
	{
		if (isUpdating)
		{
			uint8_t ack = 0xEE;
			kpX = kpX_t;
			kiX = kiX_t;
			kdX = kdX_t;
			kpW = kpW_t;
			kiW = kiW_t;
			kdW = kdW_t;
			DL_setDataWithForcedAsyncSend(DLParamRobotFeedback1, &ack);
			DL_setDataWithForcedAsyncSend(DLParamRobotFeedback2, &ack);
			return 1;
		}
		return 0;
	}
	else
		return 0;
}

void readEncoders()
{
	int16_t left;
	int16_t right;
	if (DL_getData(DLParamMotor1EncoderClicks, &left)) // ??? motor 1 is left or right
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

void readTurningSpeeds()
{
	int16_t yaw;
	DL_getData(DLParamGyroYaw, &yaw);
	float wSpeed = yaw * 2000 / 32767;
}

void drive()
{
	static float fwd_speed = 0, turn_speed = 0;
	static int16_t rightSpeedOld = 0, leftSpeedOld = 0;
	static float posXold = 0, posWold = 0;
	static int cnt = 0;
	float posX = 0, posW = 0;
	float errX, errW;
	int16_t encX, encW;
	int rightSpeed = 0, leftSpeed = 0;

	// Linear acceleration
	if(fwd_speed < speedX)
		fwd_speed += accX;
	else if(fwd_speed > speedX)
		fwd_speed -= accX;

	// Rotational acceleration
	if(turn_speed < speedW)
		turn_speed += accW;
	else if(turn_speed > speedW)
		turn_speed -= accW;

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
	leftSpeed = errX + errW;

	//  Calculate new speed values
	rightSpeedOld += rightSpeed;
	leftSpeedOld += leftSpeed;

	if(!speedX && !speedW)
	{
		rightSpeedOld = 0;
		leftSpeedOld = 0;
	}

	// Set motor speeds
	{
		int16_t limit = 500;
		leftSpeedOld = leftSpeedOld > limit ? limit : leftSpeedOld;
		leftSpeedOld = leftSpeedOld < -limit ? -limit : leftSpeedOld;
		rightSpeedOld = rightSpeedOld > limit ? limit : rightSpeedOld;
		rightSpeedOld = rightSpeedOld < -limit ? -limit : rightSpeedOld;
		int16_t rightSet = -rightSpeedOld;  // reverse right
		DL_setData(DLParamMotor1RequestSpeed, &leftSpeedOld);
		DL_setData(DLParamMotor2RequestSpeed, &rightSet);
		if (++cnt == 5)
		{
			int16_t temp = (int)fwd_speed;
			DL_setDataWithForcedAsyncSend(DLParamRobotFeedback1, &temp);
			DL_setDataWithForcedAsyncSend(DLParamRobotFeedback2, &encX);
			temp = (int)posX;
			DL_setDataWithForcedAsyncSend(DLParamRobotFeedback3, &temp);
			cnt = 0;
		}
	}
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
	readEncoders();
	readRequestedSpeeds();
	readTurningSpeeds();
	drive();
}

void Drive_setSpeed(float speedX, float speedW)
{

}

