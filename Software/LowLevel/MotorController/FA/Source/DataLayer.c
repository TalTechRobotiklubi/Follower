// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "DataLayer.h"
#include "DataLayerConfig.h"

// ----------------------------------------------------------------------------
// Variables
// ----------------------------------------------------------------------------
Bool priv_validFlags[DLNumberOfParams];

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void setDataAccordingToType(DLParam param, DLValuePointer value, Type type);
void getDataAccordingToType(DLParam param, DLValuePointer value, Type type);


/*Use this function in COMM driver layer (CAN, UART etc.) to get data from DL.
 *Param as parameter
 *pValue as pointer to place where data will be stored from DL
 *Returns if the value is valid or not*/
Boolean DL_getDataWithoutAffectingStatus(DLParam param, DLValuePointer pValue)
{
	getDataAccordingToType(param, pValue, psDLParamDescriptorList[param].eType);
	return priv_validFlags[param];
}

/*Use this function in COMM driver layer (CAN, UART etc.) to set data to DL.
 *Communication tasks (CAN_TASK etc.) are already handling data status.
 *Param as parameter
 *pValue as pointer to data for storing to DL*/
void DL_setDataWithoutAffectingStatus(DLParam param, DLValuePointer value)
{
	setDataAccordingToType(param, value, psDLParamDescriptorList[param].eType);
	priv_validFlags[param] = TRUE;
}

void DL_setDataValidity(DLParam param, Boolean validity)
{
	priv_validFlags[param] = validity;
}

/*gets data from data layer*/
void getDataAccordingToType(DLParam param, DLValuePointer value, Type type)
{
	#define GETCASE(type) case Type ## type: *((type *)value) = *((type *)psDLParamDescriptorList[param].pValue); break;

	switch(type)
	{
		GETCASE(Bool)
		GETCASE(U2)
		GETCASE(S2)
		GETCASE(U4)
		GETCASE(S4)
		GETCASE(U8)
		GETCASE(S8)
		GETCASE(U16)
		GETCASE(S16)
		GETCASE(U32)
		GETCASE(S32)
		GETCASE(Float)
		default:
			break;
	}
}

/*sets data to data layer*/
void setDataAccordingToType(DLParam param, DLValuePointer value, Type type)
{
	#define SETCASE(type) case Type ## type: *((type *)psDLParamDescriptorList[param].pValue) = *((type *)value); break;

	switch (type)
	{
		SETCASE(Bool)
		SETCASE(U2)
		SETCASE(S2)
		SETCASE(U4)
		SETCASE(S4)
		SETCASE(U8)
		SETCASE(S8)
		SETCASE(U16)
		SETCASE(S16)
		SETCASE(U32)
		SETCASE(S32)
		SETCASE(Float)
		default:
			break;
	}
}
