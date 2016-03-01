// ----------------------------------------------------------------------------
//
// Casecatcher generated datalayer configuration header file
//
// ----------------------------------------------------------------------------
#ifndef _DATALAYER_CONFIG_
#define _DATALAYER_CONFIG_

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "Typedef.h"

// ----------------------------------------------------------------------------
// Parameters
// ----------------------------------------------------------------------------
typedef enum
{
	DLParamMotor1ActualSpeed,       // 0
	DLParamMotor1EncoderClicks,     // 1
	DLParamMotor1CurrentDraw,       // 2
	DLParamMotor1DriverTemp,        // 3
	DLParamMotor1BridgeAFault,      // 4
	DLParamMotor1BridgeBFault,      // 5
	DLParamMotor2ActualSpeed,       // 6
	DLParamMotor2EncoderClicks,     // 7
	DLParamMotor2CurrentDraw,       // 8
	DLParamMotor2DriverTemp,        // 9
	DLParamMotor2BridgeAFault,      // 10
	DLParamMotor2BridgeBFault,      // 11
	DLParamRequestTranslationSpeed, // 12
	DLParamRequestRotationSpeed,    // 13
	DLParamMotor1RequestSpeed,      // 14
	DLParamMotor2RequestSpeed,      // 15
	DLParamDistanceSensor1,         // 16
	DLParamDistanceSensor2,         // 17
	DLParamDistanceSensor3,         // 18
	DLParamDistanceSensor4,         // 19
	DLParamDistanceSensor5,         // 20
	DLParamDistanceSensor6,         // 21
	DLParamDistanceSensor7,         // 22
	DLParamDistanceSensor8,         // 23
	DLParamRobotFeedback1,          // 24
	DLParamRobotFeedback2,          // 25
	DLParamRobotFeedback3,          // 26
	DLParamRobotFeedback4,          // 27
	DLParamImuQ1,                   // 28
	DLParamImuQ2,                   // 29
	DLParamImuQ3,                   // 30
	DLParamImuQ4,                   // 31
	// Count of items is 32
	DLNumberOfParams
}
DLParam;
#define DLSizeOfParam ((DLNumberOfParams <= 255) ? 1 : 2)

// ----------------------------------------------------------------------------
// General datalayer parameter value pointer
// ----------------------------------------------------------------------------
typedef void* DLValuePointer;

// ----------------------------------------------------------------------------
// Parameter descriptor
// ----------------------------------------------------------------------------
typedef struct
{
	Type eType;
	DLValuePointer pValue;
}
DLParamDescriptor;

// ----------------------------------------------------------------------------
// Export parameters descriptors
// ----------------------------------------------------------------------------
extern const DLParamDescriptor psDLParamDescriptorList[DLNumberOfParams];

#endif

