// ----------------------------------------------------------------------------
//
// Casecatcher generated datalayer configuration source file
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "DataLayerConfig.h"

// ----------------------------------------------------------------------------
// Values
// ----------------------------------------------------------------------------
Bool  pDLValueBool[2];
U8    pDLValueU8[1];
U16   pDLValueU16[1];
S16   pDLValueS16[2];

// ----------------------------------------------------------------------------
// Parameters descriptors
// ----------------------------------------------------------------------------
const DLParamDescriptor psDLParamDescriptorList[DLNumberOfParams] = 
{
	/* EmergencySetEvent       */ { TypeBool, &pDLValueBool[0] },
	/* EmergencyClearEvent     */ { TypeBool, &pDLValueBool[1] },
	/* EmergencyReserve1       */ { TypeU8,   &pDLValueU8[0]   },
	/* EmergencyReserve2       */ { TypeU16,  &pDLValueU16[0]  },
	/* RequestTranslationSpeed */ { TypeS16,  &pDLValueS16[0]  },
	/* RequestRotationSpeed    */ { TypeS16,  &pDLValueS16[1]  }
};

