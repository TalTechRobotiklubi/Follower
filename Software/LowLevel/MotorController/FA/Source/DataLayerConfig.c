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
Bool  pDLValueBool[4];
U8    pDLValueU8[4];
S16   pDLValueS16[6];

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
	/* Motor1BridgeBFault  */ { TypeBool, &pDLValueBool[1] },
	/* Motor2ActualSpeed   */ { TypeS16,  &pDLValueS16[4]  },
	/* Motor2EncoderClicks */ { TypeS16,  &pDLValueS16[5]  },
	/* Motor2CurrentDraw   */ { TypeU8,   &pDLValueU8[2]   },
	/* Motor2DriverTemp    */ { TypeU8,   &pDLValueU8[3]   },
	/* Motor2BridgeAFault  */ { TypeBool, &pDLValueBool[2] },
	/* Motor2BridgeBFault  */ { TypeBool, &pDLValueBool[3] }
};

