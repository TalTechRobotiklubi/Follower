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
U8    pDLValueBool[1];
U8    pDLValueU8[9];
S8    pDLValueS8[2];
S16   pDLValueS16[12];
Float pDLValueFloat[1];

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
	/* Qw                      */ { TypeS16,   &pDLValueS16[4]   },
	/* Qx                      */ { TypeS16,   &pDLValueS16[5]   },
	/* Qy                      */ { TypeS16,   &pDLValueS16[6]   },
	/* Qz                      */ { TypeS16,   &pDLValueS16[7]   },
	/* Motor1RequestSpeed      */ { TypeS16,   &pDLValueS16[8]   },
	/* Motor2RequestSpeed      */ { TypeS16,   &pDLValueS16[9]   },
	/* RequestTranslationSpeed */ { TypeS16,   &pDLValueS16[10]  },
	/* RequestRotationSpeed    */ { TypeS16,   &pDLValueS16[11]  },
	/* PidParameter            */ { TypeU8,    &pDLValueU8[8]    },
	/* PidValue                */ { TypeFloat, &pDLValueFloat[0] },
	/* PidUpdating             */ { TypeBool,  &pDLValueBool[0]  },
	/* CameraRequestXDegree    */ { TypeS8,    &pDLValueS8[0]    },
	/* CameraRequestZDegree    */ { TypeS8,    &pDLValueS8[1]    }
};

}
#endif
