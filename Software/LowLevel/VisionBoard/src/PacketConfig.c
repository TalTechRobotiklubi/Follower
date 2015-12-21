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
// Packet "MotorSpeeds" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketMotorSpeedsParameterList[2] = 
{
	{ DLParamMotor1RequestSpeed,  0, 16 },
	{ DLParamMotor2RequestSpeed, 16, 16 }
};

// ----------------------------------------------------------------------------
// Packet "CameraControl" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketCameraControlParameterList[2] = 
{
	{ DLParamCameraRequestXDegree, 0, 8 },
	{ DLParamCameraRequestZDegree, 8, 8 }
};

// ----------------------------------------------------------------------------
// Packets table
// ----------------------------------------------------------------------------
PacketDescriptor psPacketDescriptorList[NumberOfPackets] = 
{
	/* Motor1Status    */ { 0xD1, -1, psPacketMotor1StatusParameterList,    6, 7 },
	/* Motor2Status    */ { 0xD2, -1, psPacketMotor2StatusParameterList,    6, 7 },
	/* DistanceSensors */ { 0xF0, -1, psPacketDistanceSensorsParameterList, 8, 8 },
	/* Acceleration    */ { 0xF1, -1, psPacketAccelerationParameterList,    3, 3 },
	/* MotorSpeeds     */ { 0xD0, -1, psPacketMotorSpeedsParameterList,     2, 4 },
	/* CameraControl   */ { 0xE0, -1, psPacketCameraControlParameterList,   2, 2 }
};

