#include "SpineCmd.h"

#include <memory.h>
#include <math.h>

#define DEFAULT_KICK_TIME	20

const double SpineCmd::MaxSpeed		= 5.0;
const double SpineCmd::MaxTurnRate	= 2.0*M_PI;

SpineCmd::SpineCmd()
{
	absSpeed_	 = 0;
	radAngle_	 = 0;
	radTurnRate_ = 0;
	kick_		 = false;
	kickTime_	 = 0;
	newSpeedData_= false;
}

void SpineCmd::ClearKick(void)
{
	kickTime_	= 0;
	kick_		= false;
}

void SpineCmd::Kick(void)
{
	kick_		= true;
	kickTime_	= DEFAULT_KICK_TIME;
}

void SpineCmd::Stop()
{
	SetSpeed(0,0,0);
}

void SpineCmd::SetSpeed(double absSpeed, double radAngle, double radTurnRate, double ramp)
{
	absSpeed_		= absSpeed;
	radAngle_		= radAngle;
	radTurnRate_	= radTurnRate;

	//if(fabs(absSpeed_) > MaxSpeed || fabs(radTurnRate_) > MaxTurnRate)
	//{
	//	absSpeed_		= 0.0;
	//	radTurnRate_	= 0.0;
	//}
  
	newSpeedData_	= true;
}

SpineCmd::~SpineCmd(void)
{
}

void SpineCmd::Copy( const SpineCmd *cmd )
{
	// TODO: Or just *this = cmd would be sufficient?

	absSpeed_ = cmd->absSpeed_;
	radAngle_ = cmd->radAngle_;
	radTurnRate_	= cmd->radTurnRate_;
	newSpeedData_	= cmd->newSpeedData_;

	kick_		= cmd->kick_;
	kickTime_	= cmd->kickTime_;
}

bool SpineCmd::NewCommands( void ) const
{
	return (newSpeedData_ || kick_);
}

void SpineCmd::ClearCommands( void )
{
	newSpeedData_ = false; kick_ = false;
}

void SpineCmd::KickRegulated( KickStrength strength /*= Normal*/ )
{
	kick_		= true;
	kickTime_	= (strength == Normal ? NormalKick : StrongKick);
}
