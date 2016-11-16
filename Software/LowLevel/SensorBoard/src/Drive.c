#include "Drive.h"

#include "math.h"
#include "GPIO.h"
#include "DataLayer.h"

#include <stdlib.h>

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

static float kpX = 1.5f;
static float kiX = 0.5f;
static float kdX = 0.6f;
static float kpW = 0.9f;
static float kiW = 0.3f;
static float kdW = 0.25f;
static float accX = 2.0f;
static float accW = 3.6f;

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

static int packetFailSafeCounter = 0;

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
	int res1 = DL_getData(DLParamRequestTranslationSpeed, &read);
	priv_speedX = (float)read * 0.1;
	int res2 = DL_getData(DLParamRequestRotationSpeed, &read);
	priv_speedW = (float)read;


	if (res1 && res2)
	{
		packetFailSafeCounter = 100; // 20*20ms = 400ms
	}
	else
	{
		if (packetFailSafeCounter > 0)
			packetFailSafeCounter--;
	}
}

void readTurningSpeeds()
{
	int16_t gz;
	DL_getData(DLParamGyroZ, &gz);
	priv_gyroZ = (float)gz * 2000.0 / 32767.0;
}

float LimitVal(float val,float max)
{
	if (val > max)
	{
		return max;
	}
	else if (val < -max)
	{
		return -max;
	}
	return val;
}

int compare( const void* a, const void* b)
{
     float f_a = * ( (int*) a );
     float f_b = * ( (int*) b );

     if ( f_a == f_b ) return 0;
     else if ( f_a < f_b ) return -1;
     else return 1;
}

float filterGyroInput(float in)
{
	static float data[10] = {0,0,0,0,0,0,0,0,0,0};
	static int dp = 0;

	float sum;
	int i;

	data[dp] = in;

	dp++;
	dp %= 10;

	//sort
	qsort( data, 10, sizeof(float), compare );


	sum = 0;
	// get average
	for (i = 2; i < 8; i++)
	{
		sum += data[i];
	}

	return sum / 6;


}

void controlRangeLimit(float *val,float min,float max)
{
	if (*val  < min)
	{
		*val = min;
	}
	else if (*val  > max)
	{
		*val = min;
	}
}

void drive()
{
	static float fwd_speed = 0, turn_speed = 0;
	static int16_t rightSpeedOld = 0, leftSpeedOld = 0;
	float posX = 0, posW = 0;
	float errLX, errRX, errW;
	static float iLX = 0,iRX = 0,iW = 0;
	static float LasterrLX = 0, LasterrRX = 0, LasterrW = 0;
	float dLX = 0,dRX = 0,dW = 0;


	static float turnSpeedOld = 0;

	// Linear acceleration, TODO - do not allow negative

	if (GPIO_inputValue(STOP_BUTTON) == INPUT_OFF) //|| (!packetFailSafeCounter))
	{
		rightSpeedOld = 0,
		leftSpeedOld = 0;
		turnSpeedOld = 0;
		DL_setDataWithForcedAsyncSend(DLParamMotor1RequestSpeed, &leftSpeedOld);
		DL_setDataWithForcedAsyncSend(DLParamMotor2RequestSpeed, &rightSpeedOld);


		// reset PID
		iLX = 0;
		iRX = 0;
		iW = 0;
		LasterrLX = 0;
		LasterrRX = 0;
		LasterrW = 0;
		dLX = 0;
		dRX = 0;
		dW = 0;

		return;
	}


	if (priv_speedX == 0)
	{
		fwd_speed = priv_speedX;
	}
	else if (priv_speedX > 0)
	{

		if (fwd_speed != priv_speedX)
		{
			if (priv_speedX < fwd_speed)
			{
				fwd_speed -= accX;
			}
			else
			{
				fwd_speed += accX;
			}

			/*if (fwd_speed > priv_speedX)
			{
				fwd_speed = priv_speedX;
			}
			*/
		}
	}
	else
	{
		if (fwd_speed != priv_speedX)
		{
			if (priv_speedX > fwd_speed)
			{
				fwd_speed += accX;
			}
			else
			{
				fwd_speed -= accX;
			}

			/*
			if (fwd_speed < priv_speedX)
			{
				fwd_speed = priv_speedX;
			}
			*/

		}
	}

	/*

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
			fwd_speed = priv_speedX;
		}
		else
		{
			fwd_speed -= accX;
		}
	}

	*/
	// Rotational acceleration
	if(turn_speed < priv_speedW)
	{
		turn_speed += accW;

		if (turn_speed > priv_speedW)
		{
			turn_speed = priv_speedW;
		}

	}
	else if(turn_speed > priv_speedW)
	{
		turn_speed -= accW;

		if (turn_speed < priv_speedW)
		{
			turn_speed = priv_speedW;
		}
	}

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

	// calculate angular speed PID


	float temp_gyro = priv_gyroZ;//filterGyroInput(priv_gyroZ);

	if ((temp_gyro < 1)&&(temp_gyro > -1))
	{
		temp_gyro = 0;
	}

	errW = temp_gyro + turn_speed;
	iW += errW * 0.02 * kiW ;
	dW = (errW-LasterrW)*50;
	LasterrW = errW;
	iW = LimitVal(iW,200);


	// check if robot is suppose to move and if not then halt // rotational speed
	if ((priv_speedW == 0) && (temp_gyro == 0) && (fwd_speed == 0))
	{
		turnSpeedOld = 0;
		iW = 0;
		dW = 0;
		LasterrW = 0;
	}
	else
	{
		turnSpeedOld += (errW * kpW + iW + dW * kdW);
	}

	float encspeed = ((float)priv_leftEncoder - (float)priv_rightEncoder) / 2.0f;

	// calculate linear speed
	errLX = fwd_speed - encspeed; //+ turnSpeedOld;
	errRX = fwd_speed - encspeed; //- turnSpeedOld;

	// check if robot is suppose to move and if not then halt

	if ((errLX == 0) && (fwd_speed == 0) && (fabsf(leftSpeedOld) < 300))
	{
		errLX = 0;
		iLX = 0;
		dLX = 0;
		LasterrLX = 0;
		leftSpeedOld  = 0;// + turnSpeedOld;
	}
	else
	{
		iLX += errLX*0.02;
		iLX = LimitVal(iLX,maxI);
		dLX = (errLX-LasterrLX)*50;
		LasterrLX = errLX;
		leftSpeedOld  += (errLX * kpX + iLX * kiX + dLX * kdX);// + turnSpeedOld;
	}

	if ((errRX == 0) && (fwd_speed == 0) && (fabsf(rightSpeedOld) < 300))
	{
		errRX = 0;
		iRX = 0;
		dRX = 0;
		LasterrRX = 0;
		rightSpeedOld = 0;
	}
	else
	{
		iRX += errRX*0.02;
		iRX = LimitVal(iRX,maxI);
		dRX = (errRX-LasterrRX)*50;
		LasterrRX = errRX;
		rightSpeedOld += (errRX * kpX + iRX * kiX + dRX * kdX);// - turnSpeedOld;
	}


/*	if(!priv_speedX && !priv_speedW)
	{
		rightSpeedOld = 0;
		leftSpeedOld = 0;
	}
*/

/*	rightSpeedOld = 0;
	leftSpeedOld = 0;
*/
	int16_t speedL = (int) (leftSpeedOld  + turnSpeedOld);
	int16_t speedR = (int) (rightSpeedOld - turnSpeedOld);
	// Set motor speeds
	{
		int16_t limit = 900;
		speedL = speedL > limit ? limit : speedL;
		speedL = speedL < -limit ? -limit : speedL;
		speedR = speedR > limit ? limit : speedR;
		speedR = speedR < -limit ? -limit : speedR;
		int16_t rightSet = -speedR;  // reverse right
		DL_setDataWithForcedAsyncSend(DLParamMotor1RequestSpeed, &speedL);
		DL_setDataWithForcedAsyncSend(DLParamMotor2RequestSpeed, &rightSet);
//		if (++cnt == 10)
//		{

	//	DL_setDataWithForcedAsyncSend(DLParamRobotFeedback1, &priv_leftEncoder);
	//	DL_setDataWithForcedAsyncSend(DLParamRobotFeedback2, &priv_rightEncoder);

		//priv_leftEncoder - (float)priv_rightEncoder

			int16_t temp = (int)(encspeed * 10.0);
			DL_setDataWithForcedAsyncSend(DLParamRobotFeedback1, &temp);
			temp = (int)(fwd_speed * 10.0);
			DL_setDataWithForcedAsyncSend(DLParamRobotFeedback2, &temp);
			temp = (int)(-temp_gyro * 10.0);
			DL_setDataWithForcedAsyncSend(DLParamRobotFeedback3, &temp);
			temp = (int)(turn_speed * 10.0);
			DL_setDataWithForcedAsyncSend(DLParamRobotFeedback4, &temp);
//		}
	}
	// Save old values for PID


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

