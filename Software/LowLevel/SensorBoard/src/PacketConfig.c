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
const PacketParameter packetRobotFeedbackParameterList[4] = 
{
	{ DLParamRobotFeedback1,  0, 16 },
	{ DLParamRobotFeedback2, 16, 16 },
	{ DLParamRobotFeedback3, 32, 16 },
	{ DLParamRobotFeedback4, 48, 16 }
};

// ----------------------------------------------------------------------------
// Packet "InertialMeasurement12" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetInertialMeasurement12ParameterList[2] = 
{
	{ DLParamImuQ1,  0, 32 },
	{ DLParamImuQ2, 32, 32 }
};

// ----------------------------------------------------------------------------
// Packet "InertialMeasurement34" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetInertialMeasurement34ParameterList[2] = 
{
	{ DLParamImuQ3,  0, 32 },
	{ DLParamImuQ4, 32, 32 }
};

// ----------------------------------------------------------------------------
// Packets table
// ----------------------------------------------------------------------------
PacketDescriptor PacketDescriptorList[NumberOfPackets] = 
{
	/* Motor1Status          */ { 0xD1, 0, packetMotor1StatusParameterList,          6, 7 },
	/* Motor2Status          */ { 0xD2, 0, packetMotor2StatusParameterList,          6, 7 },
	/* RobotControl          */ { 0xD3, 0, packetRobotControlParameterList,          2, 4 },
	/* MotorSpeeds           */ { 0xD0, 0, packetMotorSpeedsParameterList,           2, 4 },
	/* DistanceSensors       */ { 0xF0, 0, packetDistanceSensorsParameterList,       8, 8 },
	/* RobotFeedback         */ { 0xD4, 0, packetRobotFeedbackParameterList,         4, 8 },
	/* InertialMeasurement12 */ { 0xF2, 0, packetInertialMeasurement12ParameterList, 2, 8 },
	/* InertialMeasurement34 */ { 0xF3, 0, packetInertialMeasurement34ParameterList, 2, 8 }
};

