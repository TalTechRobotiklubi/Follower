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
U8    pDLValueU8[8];
S8    pDLValueS8[2];
S16   pDLValueS16[16];

// ----------------------------------------------------------------------------
// Parameters descriptors
// ----------------------------------------------------------------------------
const DLParamDescriptor psDLParamDescriptorList[DLNumberOfParams] = 
{
	/* RobotFeedback1          */ { TypeS16, &pDLValueS16[0]  },
	/* RobotFeedback2          */ { TypeS16, &pDLValueS16[1]  },
	/* RobotFeedback3          */ { TypeS16, &pDLValueS16[2]  },
	/* RobotFeedback4          */ { TypeS16, &pDLValueS16[3]  },
	/* RobotFeedback5          */ { TypeS16, &pDLValueS16[4]  },
	/* RobotFeedback6          */ { TypeS16, &pDLValueS16[5]  },
	/* RobotFeedback7          */ { TypeS16, &pDLValueS16[6]  },
	/* RobotFeedback8          */ { TypeS16, &pDLValueS16[7]  },
	/* DistanceSensor1         */ { TypeU8,  &pDLValueU8[0]   },
	/* DistanceSensor2         */ { TypeU8,  &pDLValueU8[1]   },
	/* DistanceSensor3         */ { TypeU8,  &pDLValueU8[2]   },
	/* DistanceSensor4         */ { TypeU8,  &pDLValueU8[3]   },
	/* DistanceSensor5         */ { TypeU8,  &pDLValueU8[4]   },
	/* DistanceSensor6         */ { TypeU8,  &pDLValueU8[5]   },
	/* DistanceSensor7         */ { TypeU8,  &pDLValueU8[6]   },
	/* DistanceSensor8         */ { TypeU8,  &pDLValueU8[7]   },
	/* Qw                      */ { TypeS16, &pDLValueS16[8]  },
	/* Qx                      */ { TypeS16, &pDLValueS16[9]  },
	/* Qy                      */ { TypeS16, &pDLValueS16[10] },
	/* Qz                      */ { TypeS16, &pDLValueS16[11] },
	/* Motor1RequestSpeed      */ { TypeS16, &pDLValueS16[12] },
	/* Motor2RequestSpeed      */ { TypeS16, &pDLValueS16[13] },
	/* RequestTranslationSpeed */ { TypeS16, &pDLValueS16[14] },
	/* RequestRotationSpeed    */ { TypeS16, &pDLValueS16[15] },
	/* CameraRequestXDegree    */ { TypeS8,  &pDLValueS8[0]   },
	/* CameraRequestZDegree    */ { TypeS8,  &pDLValueS8[1]   }
};

