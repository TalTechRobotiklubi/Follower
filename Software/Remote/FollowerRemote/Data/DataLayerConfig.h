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
	DLParamRobotFeedback1,          // 0
	DLParamRobotFeedback2,          // 1
	DLParamRobotFeedback3,          // 2
	DLParamRobotFeedback4,          // 3
	DLParamRobotFeedback5,          // 4
	DLParamRobotFeedback6,          // 5
	DLParamRobotFeedback7,          // 6
	DLParamRobotFeedback8,          // 7
	DLParamDistanceSensor1,         // 8
	DLParamDistanceSensor2,         // 9
	DLParamDistanceSensor3,         // 10
	DLParamDistanceSensor4,         // 11
	DLParamDistanceSensor5,         // 12
	DLParamDistanceSensor6,         // 13
	DLParamDistanceSensor7,         // 14
	DLParamDistanceSensor8,         // 15
	DLParamGyroYaw,                 // 16
	DLParamGyroPitch,               // 17
	DLParamGyroRoll,                // 18
	DLParamMotor1RequestSpeed,      // 19
	DLParamMotor2RequestSpeed,      // 20
	DLParamRequestTranslationSpeed, // 21
	DLParamRequestRotationSpeed,    // 22
	DLParamPidParameter,            // 23
	DLParamPidValue,                // 24
	DLParamPidUpdating,             // 25
	DLParamCameraRequestXDegree,    // 26
	DLParamCameraRequestZDegree,    // 27
	// Count of items is 28
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
#ifdef __cplusplus
extern "C" {
extern const DLParamDescriptor psDLParamDescriptorList[DLNumberOfParams];
}
#endif

#endif

