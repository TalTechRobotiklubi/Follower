// ----------------------------------------------------------------------------
//
// Casecatcher generated packet configuration header file
//
// ----------------------------------------------------------------------------
#ifndef _PACKET_CONFIG_
#define _PACKET_CONFIG_

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "Typedef.h"
#include "DataLayerConfig.h"

// ----------------------------------------------------------------------------
// Packets enumeration
// ----------------------------------------------------------------------------
typedef enum
{
	PacketMotor1Status,          // 0
	PacketMotor2Status,          // 1
	PacketRobotControl,          // 2
	PacketMotorSpeeds,           // 3
	PacketDistanceSensors,       // 4
	PacketRobotFeedback,         // 5
	PacketInertialMeasurement12, // 6
	PacketInertialMeasurement34, // 7
	PacketPidSetup,              // 8
	// Count of items is 9
	NumberOfPackets
}
Packet;

// ----------------------------------------------------------------------------
// Packet structures
// ----------------------------------------------------------------------------
typedef struct
{
	DLParam param;
	uint16_t startBit;
	uint16_t lengthBits;
}
PacketParameter;

typedef struct
{
	uint16_t id;
	int16_t  period;
	PacketParameter const * parameterList;
	uint16_t parameterCount;
	uint16_t dlc;
}
PacketDescriptor;

// ----------------------------------------------------------------------------
// Export packet descriptors
// ----------------------------------------------------------------------------
extern PacketDescriptor PacketDescriptorList[NumberOfPackets];

#endif

