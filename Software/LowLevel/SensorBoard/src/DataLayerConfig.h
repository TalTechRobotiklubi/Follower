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
	DLParamPidParameter,            // 14
	DLParamPidValue,                // 15
	DLParamPidUpdating,             // 16
	DLParamMotor1RequestSpeed,      // 17
	DLParamMotor2RequestSpeed,      // 18
	DLParamDistanceSensor1,         // 19
	DLParamDistanceSensor2,         // 20
	DLParamDistanceSensor3,         // 21
	DLParamDistanceSensor4,         // 22
	DLParamDistanceSensor5,         // 23
	DLParamDistanceSensor6,         // 24
	DLParamDistanceSensor7,         // 25
	DLParamDistanceSensor8,         // 26
	DLParamRobotFeedback1,          // 27
	DLParamRobotFeedback2,          // 28
	DLParamRobotFeedback3,          // 29
	DLParamRobotFeedback4,          // 30
	DLParamQw,                      // 31
	DLParamQx,                      // 32
	DLParamQy,                      // 33
	DLParamQz,                      // 34
	DLParamEmergencySetEvent,       // 35
	DLParamEmergencyClearEvent,     // 36
	DLParamGyroX,                   // 37
	DLParamGyroY,                   // 38
	DLParamGyroZ,                   // 39
	// Count of items is 40
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

