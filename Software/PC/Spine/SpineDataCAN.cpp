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
}

bool SpineDataCAN::SetReceivedDataToObject(DataLayerCAN* dataLayer)
{	
	// ball sensor
	dataLayer->DL_getData(DLParamBallContact, &ballSensed_);
	// motor speeds
    dataLayer->DL_getData(DLParamMotor1ActualSpeed, &motorSpeed1_);
    dataLayer->DL_getData(DLParamMotor2ActualSpeed, &motorSpeed2_);
    dataLayer->DL_getData(DLParamMotor3ActualSpeed, &motorSpeed3_);
    /*4 buttons on the COMM module*/
    dataLayer->DL_getData(DLParamButtonTopLeft, &buttonPresses_[Top_Left_Button]);
    dataLayer->DL_getData(DLParamButtonTopRight, &buttonPresses_[Top_Right_Button]);
    dataLayer->DL_getData(DLParamButtonBottomLeft, &buttonPresses_[Bottom_Left_Button]);
    dataLayer->DL_getData(DLParamButtonBottomRight, &buttonPresses_[Bottom_Right_Button]);
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

