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
	DLParamEmergencySetEvent,       // 0
	DLParamEmergencyClearEvent,     // 1
	DLParamEmergencyReserve1,       // 2
	DLParamEmergencyReserve2,       // 3
	DLParamRequestTranslationSpeed, // 4
	DLParamRequestRotationSpeed,    // 5
	// Count of items is 6
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

