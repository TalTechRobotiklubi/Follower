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
U8    pDLValueBool[15];
U4    pDLValueU4[3];
U8    pDLValueU8[13];
U16   pDLValueU16[1];
S16   pDLValueS16[9];
U32   pDLValueU32[2];

// ----------------------------------------------------------------------------
// Parameters descriptors
// ----------------------------------------------------------------------------
const DLParamDescriptor psDLParamDescriptorList[DLNumberOfParams] = 
{
	/* Motor1ActualSpeed    */ { TypeS16,  &pDLValueS16[0]   },
	/* Motor1EncoderClicks  */ { TypeS16,  &pDLValueS16[1]   },
	/* Motor1CurrentDraw    */ { TypeU8,   &pDLValueU8[0]    },
	/* Motor1DriverTemp     */ { TypeU8,   &pDLValueU8[1]    },
	/* Motor1BridgeAFault   */ { TypeBool, &pDLValueBool[0]  },
	/* Motor1BridgeBFault   */ { TypeBool, &pDLValueBool[1]  },
	/* Motor2ActualSpeed    */ { TypeS16,  &pDLValueS16[2]   },
	/* Motor2EncoderClicks  */ { TypeS16,  &pDLValueS16[3]   },
	/* Motor2CurrentDraw    */ { TypeU8,   &pDLValueU8[2]    },
	/* Motor2DriverTemp     */ { TypeU8,   &pDLValueU8[3]    },
	/* Motor2BridgeAFault   */ { TypeBool, &pDLValueBool[2]  },
	/* Motor2BridgeBFault   */ { TypeBool, &pDLValueBool[3]  },
	/* Motor3ActualSpeed    */ { TypeS16,  &pDLValueS16[4]   },
	/* Motor3EncoderClicks  */ { TypeS16,  &pDLValueS16[5]   },
	/* Motor3CurrentDraw    */ { TypeU8,   &pDLValueU8[4]    },
	/* Motor3DriverTemp     */ { TypeU8,   &pDLValueU8[5]    },
	/* Motor3BridgeAFault   */ { TypeBool, &pDLValueBool[4]  },
	/* Motor3BridgeBFault   */ { TypeBool, &pDLValueBool[5]  },
	/* BallContact          */ { TypeBool, &pDLValueBool[6]  },
	/* BallInSight          */ { TypeBool, &pDLValueBool[7]  },
	/* BallCapacitorCharged */ { TypeBool, &pDLValueBool[8]  },
	/* ButtonTopLeft        */ { TypeBool, &pDLValueBool[9]  },
	/* ButtonTopRight       */ { TypeBool, &pDLValueBool[10] },
	/* ButtonBottomLeft     */ { TypeBool, &pDLValueBool[11] },
	/* ButtonBottomRight    */ { TypeBool, &pDLValueBool[12] },
	/* Motor1RequestSpeed   */ { TypeS16,  &pDLValueS16[6]   },
	/* Motor2RequestSpeed   */ { TypeS16,  &pDLValueS16[7]   },
	/* Motor3RequestSpeed   */ { TypeS16,  &pDLValueS16[8]   },
	/* BallCharge           */ { TypeBool, &pDLValueBool[13] },
	/* BallDribblerPower    */ { TypeU8,   &pDLValueU8[6]    },
	/* BallKickTime         */ { TypeU16,  &pDLValueU16[0]   },
	/* BallKick             */ { TypeBool, &pDLValueBool[14] },
	/* ScreenRow            */ { TypeU4,   &pDLValueU4[0]    },
	/* ScreenColumn         */ { TypeU4,   &pDLValueU4[1]    },
	/* ScreenTextLen        */ { TypeU4,   &pDLValueU4[2]    },
	/* ScreenTextChar0      */ { TypeU8,   &pDLValueU8[7]    },
	/* ScreenTextChar1      */ { TypeU8,   &pDLValueU8[8]    },
	/* ScreenTextChar2      */ { TypeU8,   &pDLValueU8[9]    },
	/* ScreenTextChar3      */ { TypeU8,   &pDLValueU8[10]   },
	/* ScreenTextChar4      */ { TypeU8,   &pDLValueU8[11]   },
	/* ScreenTextChar5      */ { TypeU8,   &pDLValueU8[12]   },
	/* ConstantAddress      */ { TypeU32,  &pDLValueU32[0]   },
	/* ConstantValue        */ { TypeU32,  &pDLValueU32[1]   }
};

}
#endif