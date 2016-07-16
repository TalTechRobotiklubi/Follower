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
Bool  pDLValueBool[5];
U8    pDLValueU8[13];
S16   pDLValueS16[23];
Float pDLValueFloat[1];

// ----------------------------------------------------------------------------
// Parameters descriptors
// ----------------------------------------------------------------------------
const DLParamDescriptor psDLParamDescriptorList[DLNumberOfParams] = 
{
	/* Motor1ActualSpeed       */ { TypeS16,   &pDLValueS16[0]   },
	/* Motor1EncoderClicks     */ { TypeS16,   &pDLValueS16[1]   },
	/* Motor1CurrentDraw       */ { TypeU8,    &pDLValueU8[0]    },
	/* Motor1DriverTemp        */ { TypeU8,    &pDLValueU8[1]    },
	/* Motor1BridgeAFault      */ { TypeBool,  &pDLValueBool[0]  },
	/* Motor1BridgeBFault      */ { TypeBool,  &pDLValueBool[1]  },
	/* Motor2ActualSpeed       */ { TypeS16,   &pDLValueS16[2]   },
	/* Motor2EncoderClicks     */ { TypeS16,   &pDLValueS16[3]   },
	/* Motor2CurrentDraw       */ { TypeU8,    &pDLValueU8[2]    },
	/* Motor2DriverTemp        */ { TypeU8,    &pDLValueU8[3]    },
	/* Motor2BridgeAFault      */ { TypeBool,  &pDLValueBool[2]  },
	/* Motor2BridgeBFault      */ { TypeBool,  &pDLValueBool[3]  },
	/* RequestTranslationSpeed */ { TypeS16,   &pDLValueS16[4]   },
	/* RequestRotationSpeed    */ { TypeS16,   &pDLValueS16[5]   },
	/* Motor1RequestSpeed      */ { TypeS16,   &pDLValueS16[6]   },
	/* Motor2RequestSpeed      */ { TypeS16,   &pDLValueS16[7]   },
	/* DistanceSensor1         */ { TypeU8,    &pDLValueU8[4]    },
	/* DistanceSensor2         */ { TypeU8,    &pDLValueU8[5]    },
	/* DistanceSensor3         */ { TypeU8,    &pDLValueU8[6]    },
	/* DistanceSensor4         */ { TypeU8,    &pDLValueU8[7]    },
	/* DistanceSensor5         */ { TypeU8,    &pDLValueU8[8]    },
	/* DistanceSensor6         */ { TypeU8,    &pDLValueU8[9]    },
	/* DistanceSensor7         */ { TypeU8,    &pDLValueU8[10]   },
	/* DistanceSensor8         */ { TypeU8,    &pDLValueU8[11]   },
	/* RobotFeedback1          */ { TypeS16,   &pDLValueS16[8]   },
	/* RobotFeedback2          */ { TypeS16,   &pDLValueS16[9]   },
	/* RobotFeedback3          */ { TypeS16,   &pDLValueS16[10]  },
	/* RobotFeedback4          */ { TypeS16,   &pDLValueS16[11]  },
	/* RobotFeedback5          */ { TypeS16,   &pDLValueS16[12]  },
	/* RobotFeedback6          */ { TypeS16,   &pDLValueS16[13]  },
	/* RobotFeedback7          */ { TypeS16,   &pDLValueS16[14]  },
	/* RobotFeedback8          */ { TypeS16,   &pDLValueS16[15]  },
	/* Qw                      */ { TypeS16,   &pDLValueS16[16]  },
	/* Qx                      */ { TypeS16,   &pDLValueS16[17]  },
	/* Qy                      */ { TypeS16,   &pDLValueS16[18]  },
	/* Qz                      */ { TypeS16,   &pDLValueS16[19]  },
	/* PidParameter            */ { TypeU8,    &pDLValueU8[12]   },
	/* PidValue                */ { TypeFloat, &pDLValueFloat[0] },
	/* PidUpdating             */ { TypeBool,  &pDLValueBool[4]  },
	/* GyroYaw                 */ { TypeS16,   &pDLValueS16[20]  },
	/* GyroPitch               */ { TypeS16,   &pDLValueS16[21]  },
	/* GyroRoll                */ { TypeS16,   &pDLValueS16[22]  }
};

