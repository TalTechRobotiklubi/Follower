#include "SpineCmdCAN.h"
//#include "KookKinematics.h"


SpineCmdCAN::SpineCmdCAN() : SpineCmd()
{
    // default speeds are 0
    motorSpeed1_ = 0;
    motorSpeed2_ = 0;
    motorSpeed3_ = 0;
    dribblerPower_ = 0;
    charge_ = 0;
    kick_ = 0;
    kickTime_ = 0;
    textRow_ = 0;
    textColumn_ = 0;
    textLength_ = 0;
    uiText_[0] = 0;
    uiText_[1] = 0;
    uiText_[2] = 0;
    uiText_[3] = 0;
    uiText_[4] = 0;
    uiText_[5] = 0;
}

void SpineCmdCAN::SetWheelSpeeds(int motorSpeed1, int motorSpeed2, int motorSpeed3)
{

    motorSpeed1_ = motorSpeed1;
    motorSpeed2_ = motorSpeed2;
    motorSpeed3_ = motorSpeed3;
    newSpeedData_	= true;
}

void SpineCmdCAN::GetWheelSpeeds(int *motorSpeed1, int *motorSpeed2, int *motorSpeed3)
{
    *motorSpeed1 = motorSpeed1_;
    *motorSpeed2 = motorSpeed2_;
    *motorSpeed3 = motorSpeed3_;
}

void SpineCmdCAN::SetKickCommandData(bool charge, int dribblerPower, int kickTime, bool kick)
{
    charge_ = charge;
    dribblerPower_ = dribblerPower;
    kickTime_ = kickTime;
    kick_ = kick;   
    newSpeedData_	= true;
}

void SpineCmdCAN::GetKickCommandData(bool *charge, int *dribblerPower, int *kickTime, bool *kick)
{
    *charge = charge_;
    *dribblerPower = dribblerPower_;
    *kickTime = kickTime_;
    *kick = kick_;    
}

void SpineCmdCAN::KickBall(void)
{
	SetKickCommandData(1, dribblerPower_, kickTime_, 1);
}

void SpineCmdCAN::DisgardKick(void)
{
	SetKickCommandData(1, dribblerPower_, kickTime_, 0);
}


void SpineCmdCAN::SetScreenWriteCommand(int row, int column, int length, const char *text)
{
    textRow_ = row;
    textColumn_ = column;
    if (length <= 6)
    {
        textLength_ = length;
    
        for (int i = 0; i < textLength_; i++)
        {
            uiText_[i] = text[i];
        }
    }
    newSpeedData_	= true;
}

void SpineCmdCAN::GetScreenWriteCommand(int *row, int *column, int *length, char *text)
{
    *row = textRow_;
    *column = textColumn_;
    *length = textLength_;
    for (int i = 0; i < *length; i++)
    {
        text[i] = uiText_[i];
    }
}

 void SpineCmdCAN::Copy(const SpineCmd *cmd)
 {
    SpineCmd::Copy(cmd);

    SpineCmdCAN *cancmd = (SpineCmdCAN*)(cmd);

    cancmd->GetWheelSpeeds(&motorSpeed1_, &motorSpeed2_, &motorSpeed3_);
    cancmd->GetKickCommandData(&charge_, &dribblerPower_, &kickTime_, &kick_);
    cancmd->GetScreenWriteCommand(&textRow_, &textColumn_, &textLength_, &uiText_[0]);
 }

 void SpineCmdCAN::SetMovement(double speed, double angle, double turnRate)
 {
    int speeds[3];
    speeds[0] = 1;
    speeds[1] = 2;
    speeds[2] = 3;
    //KookKinematics::MotorsSpeedsFromAbsSpeed(speed, angle, turnRate, speeds);
    motorSpeed1_ = speeds[0];
    motorSpeed2_ = speeds[1];
    motorSpeed3_ = speeds[2];
    newSpeedData_	= true;
 }

 void SpineCmdCAN::SetSpeed(double absSpeed, double radAngle, double radTurnRate, double ramp)
 {
    SetMovement(absSpeed, radAngle, radTurnRate);
 }