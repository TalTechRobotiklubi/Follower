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
	DLParamMotor1RequestSpeed,  // 0
	DLParamMotor2RequestSpeed,  // 1
	DLParamMotor1ActualSpeed,   // 2
	DLParamMotor1EncoderClicks, // 3
	DLParamMotor1CurrentDraw,   // 4
	DLParamMotor1DriverTemp,    // 5
	DLParamMotor1BridgeAFault,  // 6
	DLParamMotor1BridgeBFault,  // 7
	DLParamMotor2ActualSpeed,   // 8
	DLParamMotor2EncoderClicks, // 9
	DLParamMotor2CurrentDraw,   // 10
	DLParamMotor2DriverTemp,    // 11
	DLParamMotor2BridgeAFault,  // 12
	DLParamMotor2BridgeBFault,  // 13
	// Count of items is 14
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

