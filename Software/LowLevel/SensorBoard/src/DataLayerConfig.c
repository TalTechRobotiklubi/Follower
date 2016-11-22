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
Bool  pDLValueBool[7];
U8    pDLValueU8[14];
U16   pDLValueU16[1];
S16   pDLValueS16[19];
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
	/* PidParameter            */ { TypeU8,    &pDLValueU8[4]    },
	/* PidValue                */ { TypeFloat, &pDLValueFloat[0] },
	/* PidUpdating             */ { TypeBool,  &pDLValueBool[4]  },
	/* Motor1RequestSpeed      */ { TypeS16,   &pDLValueS16[6]   },
	/* Motor2RequestSpeed      */ { TypeS16,   &pDLValueS16[7]   },
	/* DistanceSensor1         */ { TypeU8,    &pDLValueU8[5]    },
	/* DistanceSensor2         */ { TypeU8,    &pDLValueU8[6]    },
	/* DistanceSensor3         */ { TypeU8,    &pDLValueU8[7]    },
	/* DistanceSensor4         */ { TypeU8,    &pDLValueU8[8]    },
	/* DistanceSensor5         */ { TypeU8,    &pDLValueU8[9]    },
	/* DistanceSensor6         */ { TypeU8,    &pDLValueU8[10]   },
	/* DistanceSensor7         */ { TypeU8,    &pDLValueU8[11]   },
	/* DistanceSensor8         */ { TypeU8,    &pDLValueU8[12]   },
	/* RobotFeedback1          */ { TypeS16,   &pDLValueS16[8]   },
	/* RobotFeedback2          */ { TypeS16,   &pDLValueS16[9]   },
	/* RobotFeedback3          */ { TypeS16,   &pDLValueS16[10]  },
	/* RobotFeedback4          */ { TypeS16,   &pDLValueS16[11]  },
	/* Qw                      */ { TypeS16,   &pDLValueS16[12]  },
	/* Qx                      */ { TypeS16,   &pDLValueS16[13]  },
	/* Qy                      */ { TypeS16,   &pDLValueS16[14]  },
	/* Qz                      */ { TypeS16,   &pDLValueS16[15]  },
	/* EmergencySetEvent       */ { TypeBool,  &pDLValueBool[5]  },
	/* EmergencyClearEvent     */ { TypeBool,  &pDLValueBool[6]  },
	/* EmergencyReserve1       */ { TypeU8,    &pDLValueU8[13]   },
	/* EmergencyReserve2       */ { TypeU16,   &pDLValueU16[0]   },
	/* GyroX                   */ { TypeS16,   &pDLValueS16[16]  },
	/* GyroY                   */ { TypeS16,   &pDLValueS16[17]  },
	/* GyroZ                   */ { TypeS16,   &pDLValueS16[18]  }
};

