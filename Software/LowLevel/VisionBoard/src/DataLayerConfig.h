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
	DLParamQw,                      // 16
	DLParamQx,                      // 17
	DLParamQy,                      // 18
	DLParamQz,                      // 19
	DLParamMotor1RequestSpeed,      // 20
	DLParamMotor2RequestSpeed,      // 21
	DLParamRequestTranslationSpeed, // 22
	DLParamRequestRotationSpeed,    // 23
	DLParamCameraRequestXDegree,    // 24
	DLParamCameraRequestZDegree,    // 25
	// Count of items is 26
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

