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
// Packet "TrackedObject" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketTrackedObjectParameterList[4] = 
{
	{ DLParamTrackedObjectX,       0, 16 },
	{ DLParamTrackedObjectY,      16, 16 },
	{ DLParamTrackedObjectHeight, 32, 16 },
	{ DLParamTrackedObjectWidth,  48, 16 }
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
// Packet "MotorPID" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketMotorPIDParameterList[2] = 
{
	{ DLParamConstantAddress,  0, 32 },
	{ DLParamConstantValue,   32, 32 }
};

// ----------------------------------------------------------------------------
// Packet "DistanceSensors" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketDistanceSensorsParameterList[8] = 
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
// Packet "Acceleration" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketAccelerationParameterList[3] = 
{
	{ DLParamAccelerationX,  0, 8 },
	{ DLParamAccelerationY,  8, 8 },
	{ DLParamAccelerationZ, 16, 8 }
};

// ----------------------------------------------------------------------------
// Packets table
// ----------------------------------------------------------------------------
const PacketDescriptor psPacketDescriptorList[NumberOfPackets] = 
{
	/* Motor1Status    */ { 0xD1, -1, psPacketMotor1StatusParameterList,    6, 7 },
	/* Motor2Status    */ { 0xD2, -1, psPacketMotor2StatusParameterList,    6, 7 },
	/* TrackedObject   */ { 0xC0, -1, psPacketTrackedObjectParameterList,   4, 8 },
	/* MotorSpeeds     */ { 0xD0, -1, psPacketMotorSpeedsParameterList,     3, 6 },
	/* MotorPID        */ { 0xD4, -1, psPacketMotorPIDParameterList,        2, 8 },
	/* DistanceSensors */ { 0xF0, -1, psPacketDistanceSensorsParameterList, 8, 8 },
	/* Acceleration    */ { 0xF1, -1, psPacketAccelerationParameterList,    3, 3 }
};
#define NUMBER_OF_PACKETS (sizeof(psPacketDescriptorList)/sizeof(Packet))

