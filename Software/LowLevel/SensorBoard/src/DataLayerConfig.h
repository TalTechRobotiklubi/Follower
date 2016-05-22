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
	DLParamRobotFeedback5,          // 28
	DLParamRobotFeedback6,          // 29
	DLParamRobotFeedback7,          // 30
	DLParamRobotFeedback8,          // 31
	DLParamGyroYaw,                 // 32
	DLParamGyroPitch,               // 33
	DLParamGyroRoll,                // 34
	DLParamPidParameter,            // 35
	DLParamPidValue,                // 36
	DLParamPidUpdating,             // 37
	DLParamQw,                      // 38
	DLParamQx,                      // 39
	DLParamQy,                      // 40
	DLParamQz,                      // 41
	// Count of items is 42
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

