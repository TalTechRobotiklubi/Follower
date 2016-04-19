// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "DataLayer.h"
#include "DataLayerConfig.h"
#include "PacketConfig.h"


// ----------------------------------------------------------------------------
// Variables
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void setDataAccordingToType(DLParam param, DLValuePointer value, Type type);
void getDataAccordingToType(DLParam param, DLValuePointer value, Type type);

void DL_init()
{
	int i = 0;
	for (; i < DLNumberOfParams; ++i)
		priv_validFlags[i] = FALSE;
}

// ----------------------------------------------------------------------------
// Use this function in logic layer to get data from DL. Handle also status for asynchronous data,
// so that after reading async data the status changes to WAITING.
// Param as parameter
// pValue as pointer to place where data will be stored from DL
// Returns if the value is valid or not
// ----------------------------------------------------------------------------
Boolean DL_getData(DLParam param, DLValuePointer pValue)
{
	uint8_t i, j;
	Bool retVal = FALSE;

	getDataAccordingToType(param, pValue, psDLParamDescriptorList[param].eType);

	/*check if parameter is part of any async packets*/
	for (i = 0; i < NumberOfPackets; i++)
	{
		PacketDescriptor *packet = &PacketDescriptorList[i];
		if (packet->period < 0)
		{
			for (j = 0; j < packet->parameterCount; j++)
			{
				if ((packet->parameterList + j)->param == param)
				{
					retVal = priv_validFlags[param];
					if (priv_validFlags[param] == TRUE)
						priv_validFlags[param] = FALSE;
					return retVal;
				}
			}
		}
	}
    return priv_validFlags[param];
}
// ----------------------------------------------------------------------------
// Use this function in logic layer to peek data from DL. Peeking does not affect the
// status of the data.
// Param as parameter
// pValue as pointer to place where data will be stored from DL
// ----------------------------------------------------------------------------
void DL_peekData(DLParam param, DLValuePointer pValue)
{
	getDataAccordingToType(param, pValue, psDLParamDescriptorList[param].eType);
}

/*Use this function in COMM driver layer (CAN, UART etc.) to get data from DL.
 *Param as parameter
 *pValue as pointer to place where data will be stored from DL
 *Returns if the value is valid or not*/
Boolean DL_getDataWithoutAffectingStatus(DLParam param, DLValuePointer pValue)
{
	getDataAccordingToType(param, pValue, psDLParamDescriptorList[param].eType);

	return priv_validFlags[param];
}

/*Use this function in logic layer to set data to DL. Handles status for asynchronous data
 *Param as parameter
 *pValue as pointer to place where data will be stored from DL*/
void DL_setData(DLParam param, DLValuePointer pValue)
{
	uint32_t tempData, newValue;
	uint32_t mask = 0;
	uint8_t i, j;
	Type type;
	uint32_t size;

    /*format data so they are comparable*/
	type = psDLParamDescriptorList[param].eType;
	getDataAccordingToType(param, &tempData, type);
	size = Type_getSize(type);
	for (i = 0; i < size; i++)
	{
		mask |= (1 << i);
	}
	tempData = tempData & mask;
	newValue = (*(uint32_t*)pValue) & mask;
	/*check if data has changed*/
    if (tempData != newValue )
	{
		setDataAccordingToType(param, pValue, type);

		/*check if parameter is part of any async packets*/
		for (i = 0; i < NumberOfPackets; i++)
		{
			PacketDescriptor* packet = &PacketDescriptorList[i];
			if (packet->period < 0)
			{
				for (j = 0; j < packet->parameterCount; j++)
				{
					if ((packet->parameterList + j)->param == param)
					{
						packet->period = PACKET_NEW;
						return;
					}
				}
			}
		}
	}
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

Type DL_getDataType(DLParam param)
{
	return psDLParamDescriptorList[param].eType;
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
