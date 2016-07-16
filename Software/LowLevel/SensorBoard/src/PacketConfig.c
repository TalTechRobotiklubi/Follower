// ----------------------------------------------------------------------------
//
// Casecatcher generated packet configuration source file
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "PacketConfig.h"

// ----------------------------------------------------------------------------
// Packet "Motor1Status" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetMotor1StatusParameterList[6] = 
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
const PacketParameter packetMotor2StatusParameterList[6] = 
{
	{ DLParamMotor2ActualSpeed,    0, 16 },
	{ DLParamMotor2EncoderClicks, 16, 16 },
	{ DLParamMotor2CurrentDraw,   32,  8 },
	{ DLParamMotor2DriverTemp,    40,  8 },
	{ DLParamMotor2BridgeAFault,  48,  1 },
	{ DLParamMotor2BridgeBFault,  49,  1 }
};

// ----------------------------------------------------------------------------
// Packet "RobotControl" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetRobotControlParameterList[2] = 
{
	{ DLParamRequestTranslationSpeed,  0, 16 },
	{ DLParamRequestRotationSpeed,    16, 16 }
};

// ----------------------------------------------------------------------------
// Packet "MotorSpeeds" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetMotorSpeedsParameterList[2] = 
{
	{ DLParamMotor1RequestSpeed,  0, 16 },
	{ DLParamMotor2RequestSpeed, 16, 16 }
};

// ----------------------------------------------------------------------------
// Packet "DistanceSensors" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetDistanceSensorsParameterList[8] = 
{
	{ DLParamDistanceSensor1,  0, 8 },
	{ DLParamDistanceSensor2,  8, 8 },
	{ DLParamDistanceSensor3, 16, 8 },
	{ DLParamDistanceSensor4, 24, 8 },
	{ DLParamDistanceSensor5, 32, 8 },
	{ DLParamDistanceSensor6, 40, 8 },
	{ DLParamDistanceSensor7, 48, 8 },
	{ DLParamDistanceSensor8, 56, 8 }
};

// ----------------------------------------------------------------------------
// Packet "RobotFeedback" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetRobotFeedbackParameterList[8] = 
{
	{ DLParamRobotFeedback1,   0, 16 },
	{ DLParamRobotFeedback2,  16, 16 },
	{ DLParamRobotFeedback3,  32, 16 },
	{ DLParamRobotFeedback4,  48, 16 },
	{ DLParamRobotFeedback5,  64, 16 },
	{ DLParamRobotFeedback6,  80, 16 },
	{ DLParamRobotFeedback7,  96, 16 },
	{ DLParamRobotFeedback8, 112, 16 }
};

// ----------------------------------------------------------------------------
// Packet "Quaternions" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetQuaternionsParameterList[4] = 
{
	{ DLParamQw,  0, 16 },
	{ DLParamQx, 16, 16 },
	{ DLParamQy, 32, 16 },
	{ DLParamQz, 48, 16 }
};

// ----------------------------------------------------------------------------
// Packet "PidSetup" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetPidSetupParameterList[3] = 
{
	{ DLParamPidParameter,  0,  8 },
	{ DLParamPidValue,      8, 32 },
	{ DLParamPidUpdating,  40,  1 }
};

// ----------------------------------------------------------------------------
// Packet "Gyro" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetGyroParameterList[3] = 
{
	{ DLParamGyroYaw,    0, 16 },
	{ DLParamGyroPitch, 16, 16 },
	{ DLParamGyroRoll,  32, 16 }
};

// ----------------------------------------------------------------------------
// Packets table
// ----------------------------------------------------------------------------
PacketDescriptor PacketDescriptorList[NumberOfPackets] =
{
	/* Motor1Status    */ { 0xD1, -1, packetMotor1StatusParameterList,    6,  7 },
	/* Motor2Status    */ { 0xD2, -1, packetMotor2StatusParameterList,    6,  7 },
	/* RobotControl    */ { 0xD3, -1, packetRobotControlParameterList,    2,  4 },
	/* MotorSpeeds     */ { 0xD0, -1, packetMotorSpeedsParameterList,     2,  4 },
	/* DistanceSensors */ { 0xF0,  0, packetDistanceSensorsParameterList, 8,  8 },
	/* RobotFeedback   */ { 0xD4, -1, packetRobotFeedbackParameterList,   8, 16 },
	/* Quaternions     */ { 0xF2,  0, packetQuaternionsParameterList,     4,  8 },
	/* PidSetup        */ { 0xD5, -1, packetPidSetupParameterList,        3,  6 },
	/* Gyro            */ { 0xF1, -1, packetGyroParameterList,            3,  6 }
};

