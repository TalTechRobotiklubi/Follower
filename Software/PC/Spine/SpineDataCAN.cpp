#include "SpineDataCAN.h"
#include "DataLayerCAN.h"

SpineDataCAN::SpineDataCAN() : SpineData()
{
    // default speeds are 0
    motorSpeed1_ = 0;
    motorSpeed2_ = 0;
    motorSpeed3_ = 0;
    // buttons not pressed
    for(int i = 0; i < 4; i++)
    {
        buttonPresses_[i] = false;
    }

    for(int i = 0; i < 6; i++)
        sensors_[i] = 0;
}

void SpineDataCAN::GetSensorData(int *sensor_data)
{
    sensor_data[0] = sensors_[0];
    sensor_data[1] = sensors_[1];
    sensor_data[2] = sensors_[2];
    sensor_data[3] = sensors_[3];
    sensor_data[4] = sensors_[4];
    sensor_data[5] = sensors_[5];


}

bool SpineDataCAN::SetReceivedDataToObject(DataLayerCAN* dataLayer)
{	
	// motor speeds
    dataLayer->DL_getData(DLParamMotor1ActualSpeed, &motorSpeed1_);
    dataLayer->DL_getData(DLParamMotor2ActualSpeed, &motorSpeed2_);
    dataLayer->DL_getData(DLParamMotor3ActualSpeed, &motorSpeed3_);
    /*4 buttons on the COMM module*/
    dataLayer->DL_getData(DLParamButtonTopLeft, &buttonPresses_[Top_Left_Button]);
    dataLayer->DL_getData(DLParamButtonTopRight, &buttonPresses_[Top_Right_Button]);
    dataLayer->DL_getData(DLParamButtonBottomLeft, &buttonPresses_[Bottom_Left_Button]);
    dataLayer->DL_getData(DLParamButtonBottomRight, &buttonPresses_[Bottom_Right_Button]);
    /* sensor data */
    dataLayer->DL_getData(DLParamDistanceSensor1, &sensors_[0]);
    dataLayer->DL_getData(DLParamDistanceSensor2, &sensors_[1]);
    dataLayer->DL_getData(DLParamDistanceSensor3, &sensors_[2]);
    dataLayer->DL_getData(DLParamDistanceSensor4, &sensors_[3]);
    dataLayer->DL_getData(DLParamDistanceSensor5, &sensors_[4]);
    dataLayer->DL_getData(DLParamDistanceSensor6, &sensors_[5]);

    /*update also start button state */
    if (buttonPresses_[Top_Left_Button] == true)
    {
        startButton_ = true;
    }
    if (buttonPresses_[Bottom_Left_Button] == true)
    {
        startButton_ = false;
    }
	/*goal selection*/
	if (buttonPresses_[Top_Right_Button] == true)
    {
//		attackGoal_ = AttackGoalType::BLUE_GOAL;
    }
    if (buttonPresses_[Bottom_Right_Button] == true)
    {
//        attackGoal_ = AttackGoalType::YELLOW_GOAL;
    }

	return true;
}

void SpineDataCAN::GetActualMotorSpeeds(int *motor1, int *motor2, int *motor3)
{
    *motor1 = motorSpeed1_;
    *motor2 = motorSpeed2_;
    *motor3 = motorSpeed3_;
}

