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
U8    pDLValueU8[8];
S16   pDLValueS16[8];
Float pDLValueFloat[4];

// ----------------------------------------------------------------------------
// Parameters descriptors
// ----------------------------------------------------------------------------
const DLParamDescriptor psDLParamDescriptorList[DLNumberOfParams] = 
{
	/* RobotFeedback1          */ { TypeS16,   &pDLValueS16[0]   },
	/* RobotFeedback2          */ { TypeS16,   &pDLValueS16[1]   },
	/* RobotFeedback3          */ { TypeS16,   &pDLValueS16[2]   },
	/* RobotFeedback4          */ { TypeS16,   &pDLValueS16[3]   },
	/* DistanceSensor1         */ { TypeU8,    &pDLValueU8[0]    },
	/* DistanceSensor2         */ { TypeU8,    &pDLValueU8[1]    },
	/* DistanceSensor3         */ { TypeU8,    &pDLValueU8[2]    },
	/* DistanceSensor4         */ { TypeU8,    &pDLValueU8[3]    },
	/* DistanceSensor5         */ { TypeU8,    &pDLValueU8[4]    },
	/* DistanceSensor6         */ { TypeU8,    &pDLValueU8[5]    },
	/* DistanceSensor7         */ { TypeU8,    &pDLValueU8[6]    },
	/* DistanceSensor8         */ { TypeU8,    &pDLValueU8[7]    },
	/* ImuQ1                   */ { TypeFloat, &pDLValueFloat[0] },
	/* ImuQ2                   */ { TypeFloat, &pDLValueFloat[1] },
	/* ImuQ3                   */ { TypeFloat, &pDLValueFloat[2] },
	/* ImuQ4                   */ { TypeFloat, &pDLValueFloat[3] },
	/* Motor1RequestSpeed      */ { TypeS16,   &pDLValueS16[4]   },
	/* Motor2RequestSpeed      */ { TypeS16,   &pDLValueS16[5]   },
	/* RequestTranslationSpeed */ { TypeS16,   &pDLValueS16[6]   },
	/* RequestRotationSpeed    */ { TypeS16,   &pDLValueS16[7]   }
};

}
#endif