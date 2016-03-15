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
U8    pDLValueU8[2];
S16   pDLValueS16[4];

// ----------------------------------------------------------------------------
// Parameters descriptors
// ----------------------------------------------------------------------------
const DLParamDescriptor psDLParamDescriptorList[DLNumberOfParams] = 
{
	/* Motor1RequestSpeed  */ { TypeS16,  &pDLValueS16[0]  },
	/* Motor2RequestSpeed  */ { TypeS16,  &pDLValueS16[1]  },
	/* Motor1ActualSpeed   */ { TypeS16,  &pDLValueS16[2]  },
	/* Motor1EncoderClicks */ { TypeS16,  &pDLValueS16[3]  },
	/* Motor1CurrentDraw   */ { TypeU8,   &pDLValueU8[0]   },
	/* Motor1DriverTemp    */ { TypeU8,   &pDLValueU8[1]   },
	/* Motor1BridgeAFault  */ { TypeBool, &pDLValueBool[0] },
	/* Motor1BridgeBFault  */ { TypeBool, &pDLValueBool[1] }
};

