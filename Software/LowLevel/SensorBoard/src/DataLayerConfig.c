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
U8    pDLValueU8[12];
S8    pDLValueS8[3];
U16   pDLValueU16[4];
S16   pDLValueS16[7];
U32   pDLValueU32[10];

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
	/* TrackedObjectX      */ { TypeU16,  &pDLValueU16[0]  },
	/* TrackedObjectY      */ { TypeU16,  &pDLValueU16[1]  },
	/* TrackedObjectHeight */ { TypeU16,  &pDLValueU16[2]  },
	/* TrackedObjectWidth  */ { TypeU16,  &pDLValueU16[3]  },
	/* Motor1RequestSpeed  */ { TypeS16,  &pDLValueS16[4]  },
	/* Motor2RequestSpeed  */ { TypeS16,  &pDLValueS16[5]  },
	/* Motor3RequestSpeed  */ { TypeS16,  &pDLValueS16[6]  },
	/* ConstantAddress     */ { TypeU32,  &pDLValueU32[0]  },
	/* ConstantValue       */ { TypeU32,  &pDLValueU32[1]  },
	/* DistanceSensor1     */ { TypeU8,   &pDLValueU8[4]   },
	/* DistanceSensor2     */ { TypeU8,   &pDLValueU8[5]   },
	/* DistanceSensor3     */ { TypeU8,   &pDLValueU8[6]   },
	/* DistanceSensor4     */ { TypeU8,   &pDLValueU8[7]   },
	/* DistanceSensor5     */ { TypeU8,   &pDLValueU8[8]   },
	/* DistanceSensor6     */ { TypeU8,   &pDLValueU8[9]   },
	/* DistanceSensor7     */ { TypeU8,   &pDLValueU8[10]  },
	/* DistanceSensor8     */ { TypeU8,   &pDLValueU8[11]  },
	/* AccelerationX       */ { TypeS8,   &pDLValueS8[0]   },
	/* AccelerationY       */ { TypeS8,   &pDLValueS8[1]   },
	/* AccelerationZ       */ { TypeS8,   &pDLValueS8[2]   },
	/* DepthRaw0           */ { TypeU32,  &pDLValueU32[2]  },
	/* DepthRaw1           */ { TypeU32,  &pDLValueU32[3]  },
	/* DepthRaw2           */ { TypeU32,  &pDLValueU32[4]  },
	/* DepthRaw3           */ { TypeU32,  &pDLValueU32[5]  },
	/* DepthRaw4           */ { TypeU32,  &pDLValueU32[6]  },
	/* DepthRaw5           */ { TypeU32,  &pDLValueU32[7]  },
	/* DepthRaw6           */ { TypeU32,  &pDLValueU32[8]  },
	/* DepthRaw7           */ { TypeU32,  &pDLValueU32[9]  }
};

