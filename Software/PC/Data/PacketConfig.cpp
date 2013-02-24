// ----------------------------------------------------------------------------
//
// Casecatcher generated packet configuration source file
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "PacketConfig.h"

#ifdef __cplusplus
extern "C" {
// ----------------------------------------------------------------------------
// Packet "Motor1Status" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketMotor1StatusParameterList[6] = 
{
	{ DLParamMotor1ActualSpeed,    0, 16 },
	{ DLParamMotor1EncoderClicks, 16, 16 },
	{ DLParamMotor1CurrentDraw,   32,  8 },
	{ DLParamMotor1DriverTemp,    40,  8 },
	{ DLParamMotor1BridgeAFault,  48,  1 },
	{ DLParamMotor1BridgeBFault,  49,  1 }
};

// ----------------------------------------------------------------------------
// Packet "Motor2Status" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketMotor2StatusParameterList[6] = 
{
	{ DLParamMotor2ActualSpeed,    0, 16 },
	{ DLParamMotor2EncoderClicks, 16, 16 },
	{ DLParamMotor2CurrentDraw,   32,  8 },
	{ DLParamMotor2DriverTemp,    40,  8 },
	{ DLParamMotor2BridgeAFault,  48,  1 },
	{ DLParamMotor2BridgeBFault,  49,  1 }
};

// ----------------------------------------------------------------------------
// Packet "Motor3Status" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketMotor3StatusParameterList[6] = 
{
	{ DLParamMotor3ActualSpeed,    0, 16 },
	{ DLParamMotor3EncoderClicks, 16, 16 },
	{ DLParamMotor3CurrentDraw,   32,  8 },
	{ DLParamMotor3DriverTemp,    40,  8 },
	{ DLParamMotor3BridgeAFault,  48,  1 },
	{ DLParamMotor3BridgeBFault,  49,  1 }
};

// ----------------------------------------------------------------------------
// Packet "BallStatus" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketBallStatusParameterList[3] = 
{
	{ DLParamBallContact,          0, 1 },
	{ DLParamBallInSight,          1, 1 },
	{ DLParamBallCapacitorCharged, 2, 1 }
};

// ----------------------------------------------------------------------------
// Packet "UIControls" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketUIControlsParameterList[4] = 
{
	{ DLParamButtonTopLeft,     0, 1 },
	{ DLParamButtonTopRight,    1, 1 },
	{ DLParamButtonBottomLeft,  2, 1 },
	{ DLParamButtonBottomRight, 3, 1 }
};

// ----------------------------------------------------------------------------
// Packet "MotorSpeeds" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketMotorSpeedsParameterList[3] = 
{
	{ DLParamMotor1RequestSpeed,  0, 16 },
	{ DLParamMotor2RequestSpeed, 16, 16 },
	{ DLParamMotor3RequestSpeed, 32, 16 }
};

// ----------------------------------------------------------------------------
// Packet "BallControl" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketBallControlParameterList[4] = 
{
	{ DLParamBallCharge,         0,  1 },
	{ DLParamBallDribblerPower,  1,  7 },
	{ DLParamBallKickTime,       8, 10 },
	{ DLParamBallKick,          18,  1 }
};

// ----------------------------------------------------------------------------
// Packet "UIScreenWrite" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketUIScreenWriteParameterList[9] = 
{
	{ DLParamScreenRow,        0, 4 },
	{ DLParamScreenColumn,     4, 4 },
	{ DLParamScreenTextLen,    8, 4 },
	{ DLParamScreenTextChar0, 16, 8 },
	{ DLParamScreenTextChar1, 24, 8 },
	{ DLParamScreenTextChar2, 32, 8 },
	{ DLParamScreenTextChar3, 40, 8 },
	{ DLParamScreenTextChar4, 48, 8 },
	{ DLParamScreenTextChar5, 56, 8 }
};

// ----------------------------------------------------------------------------
// Packet "MotorPID" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketMotorPIDParameterList[2] = 
{
	{ DLParamConstantAddress,  0, 32 },
	{ DLParamConstantValue,   32, 32 }
};

// ----------------------------------------------------------------------------
// Packets table
// ----------------------------------------------------------------------------
const PacketDescriptor psPacketDescriptorList[NumberOfPackets] = 
{
	/* Motor1Status  */ { 0xD1, 0, psPacketMotor1StatusParameterList,  6, 7 },
	/* Motor2Status  */ { 0xD2, 0, psPacketMotor2StatusParameterList,  6, 7 },
	/* Motor3Status  */ { 0xD3, 0, psPacketMotor3StatusParameterList,  6, 7 },
	/* BallStatus    */ { 0xB1, 0, psPacketBallStatusParameterList,    3, 1 },
	/* UIControls    */ { 0xC0, 0, psPacketUIControlsParameterList,    4, 1 },
	/* MotorSpeeds   */ { 0xD0, -1, psPacketMotorSpeedsParameterList,   3, 6 },
	/* BallControl   */ { 0xB0, -1, psPacketBallControlParameterList,   4, 3 },
	/* UIScreenWrite */ { 0xC3, -1, psPacketUIScreenWriteParameterList, 9, 8 },
	/* MotorPID      */ { 0xD4, 0, psPacketMotorPIDParameterList,      2, 8 }
};
#define NUMBER_OF_PACKETS (sizeof(psPacketDescriptorList)/sizeof(Packet))

}
#endif