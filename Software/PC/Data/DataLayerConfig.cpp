// ----------------------------------------------------------------------------
//
// Casecatcher generated datalayer configuration source file
//
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "DataLayerConfig.h"

#ifdef __cplusplus
extern "C" {
// ----------------------------------------------------------------------------
// Values
// ----------------------------------------------------------------------------
U8    pDLValueBool[10];
U4    pDLValueU4[3];
U8    pDLValueU8[18];
S16   pDLValueS16[9];
U32   pDLValueU32[2];

// ----------------------------------------------------------------------------
// Parameters descriptors
// ----------------------------------------------------------------------------
const DLParamDescriptor psDLParamDescriptorList[DLNumberOfParams] = 
{
	/* Motor1ActualSpeed   */ { TypeS16,  &pDLValueS16[0]  },
	/* Motor1EncoderClicks */ { TypeS16,  &pDLValueS16[1]  },
	/* Motor1CurrentDraw   */ { TypeU8,   &pDLValueU8[0]   },
	/* Motor1DriverTemp    */ { TypeU8,   &pDLValueU8[1]   },
	/* Motor1BridgeAFault  */ { TypeBool, &pDLValueBool[0] },
	/* Motor1BridgeBFault  */ { TypeBool, &pDLValueBool[1] },
	/* Motor2ActualSpeed   */ { TypeS16,  &pDLValueS16[2]  },
	/* Motor2EncoderClicks */ { TypeS16,  &pDLValueS16[3]  },
	/* Motor2CurrentDraw   */ { TypeU8,   &pDLValueU8[2]   },
	/* Motor2DriverTemp    */ { TypeU8,   &pDLValueU8[3]   },
	/* Motor2BridgeAFault  */ { TypeBool, &pDLValueBool[2] },
	/* Motor2BridgeBFault  */ { TypeBool, &pDLValueBool[3] },
	/* Motor3ActualSpeed   */ { TypeS16,  &pDLValueS16[4]  },
	/* Motor3EncoderClicks */ { TypeS16,  &pDLValueS16[5]  },
	/* Motor3CurrentDraw   */ { TypeU8,   &pDLValueU8[4]   },
	/* Motor3DriverTemp    */ { TypeU8,   &pDLValueU8[5]   },
	/* Motor3BridgeAFault  */ { TypeBool, &pDLValueBool[4] },
	/* Motor3BridgeBFault  */ { TypeBool, &pDLValueBool[5] },
	/* ButtonTopLeft       */ { TypeBool, &pDLValueBool[6] },
	/* ButtonTopRight      */ { TypeBool, &pDLValueBool[7] },
	/* ButtonBottomLeft    */ { TypeBool, &pDLValueBool[8] },
	/* ButtonBottomRight   */ { TypeBool, &pDLValueBool[9] },
	/* DistanceSensor1     */ { TypeU8,   &pDLValueU8[6]   },
	/* DistanceSensor2     */ { TypeU8,   &pDLValueU8[7]   },
	/* DistanceSensor3     */ { TypeU8,   &pDLValueU8[8]   },
	/* DistanceSensor4     */ { TypeU8,   &pDLValueU8[9]   },
	/* DistanceSensor5     */ { TypeU8,   &pDLValueU8[10]  },
	/* DistanceSensor6     */ { TypeU8,   &pDLValueU8[11]  },
	/* Motor1RequestSpeed  */ { TypeS16,  &pDLValueS16[6]  },
	/* Motor2RequestSpeed  */ { TypeS16,  &pDLValueS16[7]  },
	/* Motor3RequestSpeed  */ { TypeS16,  &pDLValueS16[8]  },
	/* ScreenRow           */ { TypeU4,   &pDLValueU4[0]   },
	/* ScreenColumn        */ { TypeU4,   &pDLValueU4[1]   },
	/* ScreenTextLen       */ { TypeU4,   &pDLValueU4[2]   },
	/* ScreenTextChar0     */ { TypeU8,   &pDLValueU8[12]  },
	/* ScreenTextChar1     */ { TypeU8,   &pDLValueU8[13]  },
	/* ScreenTextChar2     */ { TypeU8,   &pDLValueU8[14]  },
	/* ScreenTextChar3     */ { TypeU8,   &pDLValueU8[15]  },
	/* ScreenTextChar4     */ { TypeU8,   &pDLValueU8[16]  },
	/* ScreenTextChar5     */ { TypeU8,   &pDLValueU8[17]  },
	/* ConstantAddress     */ { TypeU32,  &pDLValueU32[0]  },
	/* ConstantValue       */ { TypeU32,  &pDLValueU32[1]  }
};

}
#endif