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
	DLParamDistanceSensor1,         // 4
	DLParamDistanceSensor2,         // 5
	DLParamDistanceSensor3,         // 6
	DLParamDistanceSensor4,         // 7
	DLParamDistanceSensor5,         // 8
	DLParamDistanceSensor6,         // 9
	DLParamDistanceSensor7,         // 10
	DLParamDistanceSensor8,         // 11
	DLParamImuQ1,                   // 12
	DLParamImuQ2,                   // 13
	DLParamImuQ3,                   // 14
	DLParamImuQ4,                   // 15
	DLParamMotor1RequestSpeed,      // 16
	DLParamMotor2RequestSpeed,      // 17
	DLParamRequestTranslationSpeed, // 18
	DLParamRequestRotationSpeed,    // 19
	// Count of items is 20
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

