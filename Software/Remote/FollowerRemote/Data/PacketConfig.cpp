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
// Packet "MotorSpeeds" parameters table
// ----------------------------------------------------------------------------
const PacketParameter packetMotorSpeedsParameterList[2] = 
{
	{ DLParamMotor1RequestSpeed,  0, 16 },
	{ DLParamMotor2RequestSpeed, 16, 16 }
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
	/* RobotFeedback         */ { 0xD4, 0, packetRobotFeedbackParameterList,         4, 8 },
	/* DistanceSensors       */ { 0xF0, 0, packetDistanceSensorsParameterList,       8, 8 },
	/* InertialMeasurement12 */ { 0xF2, 0, packetInertialMeasurement12ParameterList, 2, 8 },
	/* InertialMeasurement34 */ { 0xF3, 0, packetInertialMeasurement34ParameterList, 2, 8 },
	/* MotorSpeeds           */ { 0xD0, 0, packetMotorSpeedsParameterList,           2, 4 },
	/* RobotControl          */ { 0xD3, 0, packetRobotControlParameterList,          2, 4 }
};
#define NUMBER_OF_PACKETS (sizeof(psPacketDescriptorList)/sizeof(Packet))

}
#endif
