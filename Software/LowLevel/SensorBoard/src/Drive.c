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

static float enc_fix = 1.025f;
static float kpX = 0.5f;
static float kiX = 0.0f;
static float kdX = 0.0f;
static float kpW = 0.3f;
static float kiW = 0.0f;
static float kdW = 0.0f;
static float accX = 2.0f;
static float accW = 2.0f;

static float kpX_t = 0.0f;  // temporary fields
static float kiX_t = 0.0f;
static float kdX_t = 0.0f;
static float kpW_t = 0.0f;
static float kiW_t = 0.0f;
static float kdW_t = 0.0f;
static float accX_t = 0.0f;
static float accW_t = 0.0f;

static int16_t priv_leftEncoder = 0;  //are 32-bits enough
static int16_t priv_rightEncoder = 0;
static float priv_speedX = 0;
static float priv_speedW = 0;
static float priv_gyroZ = 0;

static uint8_t updateParameters();
static void readEncoders();
static void stop();
static void readRequestedSpeeds();
static void readTurningSpeeds();
static void drive();

#define maxI 1000

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
	// TODO! Fail safe to add if one motor feedback not received then stop another
	int16_t left;
	int16_t right;
	if (DL_getData(DLParamMotor1EncoderClicks, &left)) // motor 1 is left
		priv_leftEncoder = left;
	if (DL_getData(DLParamMotor2EncoderClicks, &right))
		priv_rightEncoder = right;
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
	priv_speedX = (float)read/10.0;
	DL_getData(DLParamRequestRotationSpeed, &read);
	priv_speedW = (float)read;
}

void readTurningSpeeds()
{
	int16_t gz;
	DL_getData(DLParamGyroZ, &gz);
	priv_gyroZ = gz * 2000 / 32767;
}

void drive()
{
	static float fwd_speed = 0, turn_speed = 0;
	static int16_t rightSpeedOld = 0, leftSpeedOld = 0;
	static float posXold = 0, posWold = 0;
	static int cnt = 0;
	float posX = 0, posW = 0;
	float errLX, errRX, errW;
	static float iLX = 0,iRX = 0,iW = 0;
	static float LasterrLX = 0, LasterrRX = 0, LasterrW = 0;
	float dLX = 0,dRX = 0,dW = 0;
	int16_t encX, encW;
	int rightSpeed = 0, leftSpeed = 0;

	int16_t turnSpeedOld = 0;

	// Linear acceleration, TODO - do not allow negative




	if(fwd_speed < priv_speedX)
	{
		if (priv_speedX >= 0)
		{
			fwd_speed += accX;
		}
		else
		{
			fwd_speed = priv_speedX;
		}

	}
	else if(fwd_speed > priv_speedX)
	{
		if (priv_speedX >= 0)
		{
			fwd_speed -= priv_speedX;
		}
		else
		{
			fwd_speed -= accX;
		}
	}
	// Rotational acceleration
	if(turn_speed < priv_speedW)
		turn_speed += accW;
	else if(turn_speed > priv_speedW)
		turn_speed -= accW;


//	if (priv_speedW > 0)
//	{
//		int16_t rSet = priv_speedW;  // reverse right
//		int16_t lSet = priv_speedW;
//		DL_setDataWithForcedAsyncSend(DLParamMotor1RequestSpeed, &lSet);
//		DL_setDataWithForcedAsyncSend(DLParamMotor2RequestSpeed, &rSet);
//		priv_leftEncoder = 0;
//	    priv_rightEncoder = 0;
//	    return;
//	}
//	else if (priv_speedW < 0)
//	{
//		int16_t rSet = priv_speedW;  // reverse right
//		int16_t lSet = priv_speedW;
//		DL_setDataWithForcedAsyncSend(DLParamMotor1RequestSpeed, &lSet);
//		DL_setDataWithForcedAsyncSend(DLParamMotor2RequestSpeed, &rSet);
//		priv_leftEncoder = 0;
//		priv_rightEncoder = 0;
//		return;
//	}

	// calculate linear and angular speed
	errLX = fwd_speed - priv_leftEncoder;
	errRX = enc_fix * fwd_speed + priv_rightEncoder;
	errW = priv_gyroZ + priv_speedW;

	iLX += errLX;
	iRX += errRX;
	iW += errW;

	dLX = LasterrLX-errLX;
	dRX = LasterrRX-errRX;
	dW = LasterrW-errW;

	LasterrLX = errLX;
	LasterrRX = errRX;
	LasterrW = errW;

	if (iLX <  -maxI)
		iLX = -maxI;

	if (iLX >  maxI)
		iLX = maxI;

	if (iRX <  -maxI)
		iRX= -maxI;

	if (iRX >  maxI)
		iRX = maxI;

	if (iW <  -maxI)
		iW= -maxI;

	if (iW >  maxI)
		iW = maxI;

	//encW = (priv_rightEncoder - priv_leftEncoder);

	// calculate linear and angular speed errors
	//posX = fwd_speed - encX;
	//posW = turn_speed - encW;

	// Calculate driving errors for PID
	//errX = kpX * posX + kdX * (posX - posXold);
	//errW = kpW * posW + kdW * (posW - posWold);

//	leftSpeed = errX - errW;
//	rightSpeed = errX + errW;

	//  Calculate new speed values

	turnSpeedOld = (errW * kpW + iW * kiW + dW * kdW);
	leftSpeedOld  += (errLX * kpX + iLX * kiX + dLX * kdX) + turnSpeedOld;
	rightSpeedOld += (errRX * kpX + iRX * kiX + dRX * kdX) - turnSpeedOld;

	if(!priv_speedX && !priv_speedW)
	{
		rightSpeedOld = 0;
		leftSpeedOld = 0;
	}

	// Set motor speeds
	{
		int16_t limit = 900;
		leftSpeedOld = leftSpeedOld > limit ? limit : leftSpeedOld;
		leftSpeedOld = leftSpeedOld < -limit ? -limit : leftSpeedOld;
		rightSpeedOld = rightSpeedOld > limit ? limit : rightSpeedOld;
		rightSpeedOld = rightSpeedOld < -limit ? -limit : rightSpeedOld;
		int16_t rightSet = -rightSpeedOld;  // reverse right
		DL_setDataWithForcedAsyncSend(DLParamMotor1RequestSpeed, &leftSpeedOld);
		DL_setDataWithForcedAsyncSend(DLParamMotor2RequestSpeed, &rightSet);
//		if (++cnt == 10)
//		{
			int16_t temp = (int)errW;
			DL_setDataWithForcedAsyncSend(DLParamRobotFeedback1, &turnSpeedOld);
			temp = (int)iW;
			DL_setDataWithForcedAsyncSend(DLParamRobotFeedback2, &leftSpeedOld);
			temp = (int)(kiW*100);
			DL_setDataWithForcedAsyncSend(DLParamRobotFeedback3, &rightSpeedOld);
			cnt = 0;
//		}
	}
	// Save old values for PID
	posXold = posX;
	posWold = posW;

	//priv_leftEncoder = 0;
	//priv_rightEncoder = 0;
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

