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
// Packet "RobotFeedback" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetRobotFeedbackParameterList[4] = 
{
	{ DLParamRobotFeedback1,  0, 16 },
	{ DLParamRobotFeedback2, 16, 16 },
	{ DLParamRobotFeedback3, 32, 16 },
	{ DLParamRobotFeedback4, 48, 16 }
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
// Packet "MotorSpeeds" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetMotorSpeedsParameterList[2] = 
{
	{ DLParamMotor1RequestSpeed,  0, 16 },
	{ DLParamMotor2RequestSpeed, 16, 16 }
};

// ----------------------------------------------------------------------------
// Packet "CameraControl" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetCameraControlParameterList[2] = 
{
	{ DLParamCameraRequestXDegree, 0, 8 },
	{ DLParamCameraRequestZDegree, 8, 8 }
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
// Packets table
// ----------------------------------------------------------------------------
PacketDescriptor PacketDescriptorList[NumberOfPackets] = 
{
	/* RobotFeedback   */ { 0xD4, packetRobotFeedbackParameterList,   4, 8 },
	/* DistanceSensors */ { 0xF0, packetDistanceSensorsParameterList, 8, 8 },
	/* Quaternions     */ { 0xF2, packetQuaternionsParameterList,     4, 8 },
	/* MotorSpeeds     */ { 0xD0, packetMotorSpeedsParameterList,     2, 4 },
	/* CameraControl   */ { 0xE0, packetCameraControlParameterList,   2, 2 },
	/* RobotControl    */ { 0xD3, packetRobotControlParameterList,    2, 4 }
};

