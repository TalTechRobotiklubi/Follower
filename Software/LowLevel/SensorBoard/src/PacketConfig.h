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
	PacketMotor1Status,    // 0
	PacketMotor2Status,    // 1
	PacketRobotControl,    // 2
	PacketPidSetup,        // 3
	PacketMotorSpeeds,     // 4
	PacketDistanceSensors, // 5
	PacketRobotFeedback,   // 6
	PacketQuaternions,     // 7
	PacketEmergency,       // 8
	PacketGyro,            // 9
	// Count of items is 10
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

