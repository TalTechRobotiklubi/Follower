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
// Packet "DepthRaw" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketDepthRawParameterList[8] = 
{
	{ DLParamDepthRaw0,   0, 32 },
	{ DLParamDepthRaw1,  32, 32 },
	{ DLParamDepthRaw2,  64, 32 },
	{ DLParamDepthRaw3,  96, 32 },
	{ DLParamDepthRaw4, 128, 32 },
	{ DLParamDepthRaw5, 160, 32 },
	{ DLParamDepthRaw6, 192, 32 },
	{ DLParamDepthRaw7, 224, 32 }
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
// Packets table
// ----------------------------------------------------------------------------
const PacketDescriptor psPacketDescriptorList[NumberOfPackets] = 
{
	/* Motor1Status    */ { 0xD1, 0, psPacketMotor1StatusParameterList,    6,  7 },
	/* Motor2Status    */ { 0xD2, 0, psPacketMotor2StatusParameterList,    6,  7 },
	/* DistanceSensors */ { 0xF0, 0, psPacketDistanceSensorsParameterList, 8,  8 },
	/* Acceleration    */ { 0xF1, 0, psPacketAccelerationParameterList,    3,  3 },
    /* TrackedObject   */ { 0xC0, -1, psPacketTrackedObjectParameterList,   4,  8 },
    /* DepthRaw        */ { 0xC1, -1, psPacketDepthRawParameterList,        8, 32 },
	/* MotorSpeeds     */ { 0xD0, 15, psPacketMotorSpeedsParameterList,    3,  6 },
	/* MotorPID        */ { 0xD4, 0, psPacketMotorPIDParameterList,        2,  8 }
};
#define NUMBER_OF_PACKETS (sizeof(psPacketDescriptorList)/sizeof(Packet))

}
#endif
