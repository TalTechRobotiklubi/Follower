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
// Packet "MotorSpeeds" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketMotorSpeedsParameterList[3] = 
{
	{ DLParamMotor1RequestSpeed,  0, 16 },
	{ DLParamMotor2RequestSpeed, 16, 16 },
	{ DLParamMotor3RequestSpeed, 32, 16 }
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
// Packet "Sensors" parameters table
// ----------------------------------------------------------------------------
const PacketParameter psPacketSensorsParameterList[8] = 
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
// Packets table
// ----------------------------------------------------------------------------
const PacketDescriptor psPacketDescriptorList[NumberOfPackets] = 
{
	/* Motor1Status  */ { 0xD1,  -1, psPacketMotor1StatusParameterList,  6, 7 },
	/* Motor2Status  */ { 0xD2,  -1, psPacketMotor2StatusParameterList,  6, 7 },
	/* Motor3Status  */ { 0xD3,  -1, psPacketMotor3StatusParameterList,  6, 7 },
	/* MotorSpeeds   */ { 0xD0,  -1, psPacketMotorSpeedsParameterList,   3, 6 },
	/* UIScreenWrite */ { 0xC3,  -1, psPacketUIScreenWriteParameterList, 9, 8 },
	/* MotorPID      */ { 0xD4,  -1, psPacketMotorPIDParameterList,      2, 8 },
	/* UIControls    */ { 0xC0,  -1, psPacketUIControlsParameterList,    4, 1 },
	/* Sensors       */ { 0xF0,  -1, psPacketSensorsParameterList,       8, 8 }
};
#define NUMBER_OF_PACKETS (sizeof(psPacketDescriptorList)/sizeof(Packet))

