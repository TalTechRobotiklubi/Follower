#pragma once

#include "SpineCmd.h"

class SpineCmdCAN : public SpineCmd
{
public:
	SpineCmdCAN();

	// 0xD0 command
    virtual void SetSpeed(double absSpeed, double radAngle, double radTurnRate, double ramp = false);
    
    void SetWheelSpeeds(int motorSpeed1, int motorSpeed2, int motorSpeed3);
    void GetWheelSpeeds(int *motorSpeed1, int *motorSpeed2, int *motorSpeed3);
    // 0xB0 command
    void SetKickCommandData(bool charge, int dribblerPower, int kickTime, bool kick);
    void GetKickCommandData(bool *charge, int *dribblerPower, int *kickTime, bool *kick);
	void KickBall();
	void DisgardKick();
    // 0xC3 command (UIScreenWrite)
    void SetScreenWriteCommand(int row, int column, int length, const char *text);
    void GetScreenWriteCommand(int *row, int *column, int *length, char *text);

    void	Copy(const SpineCmd *cmd);

private:
    void SetMovement(double speed, double angle, double turnRate);

    int             motorSpeed1_;
    int             motorSpeed2_;
    int             motorSpeed3_;
    int             dribblerPower_;
    bool            charge_;
    int             textRow_;
    int             textColumn_;
    int             textLength_;
    char            uiText_[6];
};