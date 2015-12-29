// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "dataLayer.h"
#include "datalayerconfig.h"
#include "packetconfig.h"

static void SetDataAccordingToType(DLParam param, DLValuePointer value, Type type);
static void GetDataAccordingToType(DLParam param, DLValuePointer value, Type type);

static bool pbValidFlags_[DLNumberOfParams];

void DL_init()
{
    for (int i = 0; i < DLNumberOfParams; ++i)
        pbValidFlags_[i] = false;
}

// ----------------------------------------------------------------------------
// Use this function in logic layer to get data from DL. Handle also status for asynchronous data,
// so that after reading async data the status changes to WAITING.
// Param as parameter
// pValue as pointer to place where data will be stored from DL
// Returns if the value is valid or not
// ----------------------------------------------------------------------------
bool DL_getData(DLParam param, DLValuePointer pValue)
{
	uint8_t i, j;

	GetDataAccordingToType(param, pValue, psDLParamDescriptorList[param].eType);

	/*check if parameter is part of any async packets*/
	for (i = 0; i < NumberOfPackets; i++)
	{
		PacketDescriptor *packet = &psPacketDescriptorList[i];
		if (packet->iPeriod < 0)
		{
			for (j = 0; j < packet->uiParameterCount; j++)
			{
				if ((packet->psParameterList + j)->eParam == param)
				{
					if (packet->iPeriod != PACKET_WAITING)
					{
						packet->iPeriod = PACKET_WAITING;
                        pbValidFlags_[param] = true;
					}
                    else
                        pbValidFlags_[param] = false;
                    break;
				}
			}
		}
	}
    return pbValidFlags_[param];
}
// ----------------------------------------------------------------------------
// Use this function in logic layer to peek data from DL. Peeking does not affect the
// status of the data.
// Param as parameter
// pValue as pointer to place where data will be stored from DL
// ----------------------------------------------------------------------------
void DL_peekData(DLParam param, DLValuePointer pValue)
{
	GetDataAccordingToType(param, pValue, psDLParamDescriptorList[param].eType);
}

/*Use this function in COMM driver layer (CAN, UART etc.) to get data from DL.
 *Param as parameter
 *pValue as pointer to place where data will be stored from DL
 *Returns if the value is valid or not*/
bool DL_getDataWithoutAffectingStatus(DLParam param, DLValuePointer pValue)
{
	GetDataAccordingToType(param, pValue, psDLParamDescriptorList[param].eType);

	return pbValidFlags_[param];
}

/*Use this function in logic layer to set data to DL. Handles status for asynchronous data
 *Param as parameter
 *pValue as pointer to place where data will be stored from DL*/
void DL_setData(DLParam param, DLValuePointer pValue)
{
	uint32_t tempData;
	uint32_t newValue;
    uint32_t mask = 0;
	uint32_t size;
	uint8_t i, j;
    Type type;

    /*format data so they are comparable*/
	type = psDLParamDescriptorList[param].eType;
	GetDataAccordingToType(param, &tempData, type);
	size = psTypeSize[type];
	for (i = 0; i < size; i++)
	{
		mask |= (1 << i);
	}
	tempData = tempData & mask;
	newValue = (*(uint32_t*)pValue) & mask;
	/*check if data has changed*/
    if (tempData != newValue )
	{
		SetDataAccordingToType(param, pValue, type);

		/*check if parameter is part of any async packets*/
		for (i = 0; i < NumberOfPackets; i++)
		{
			PacketDescriptor* packet = &psPacketDescriptorList[i];
			if (packet->iPeriod < 0)
			{
				for (j = 0; j < packet->uiParameterCount; j++)
				{
					if ((packet->psParameterList + j)->eParam == param)
					{
						packet->iPeriod = PACKET_READY_TO_SEND;
						break;
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
	SetDataAccordingToType(param, value, psDLParamDescriptorList[param].eType);
	
	pbValidFlags_[param] = true;
}

Type DL_getDataType(DLParam param)
{
	return psDLParamDescriptorList[param].eType;
}

void DL_setDataValidity(DLParam param, bool validity)
{
	pbValidFlags_[param] = validity;
}

/*gets data from data layer*/
void GetDataAccordingToType(DLParam param, DLValuePointer value, Type type)
{
	#define GETCASE(type) case Type ## type: *((type *)value) = *((type *)psDLParamDescriptorList[param].pValue); break;

	switch(type)
	{
		case TypeBool:
            *((U8 *)value) = *((U8 *)psDLParamDescriptorList[param].pValue);
            break;
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
void SetDataAccordingToType(DLParam param, DLValuePointer value, Type type)
{
	#define SETCASE(type) case Type ## type: *((type *)psDLParamDescriptorList[param].pValue) = *((type *)value); break;

	switch (type)
	{
		case TypeBool:
            *((U8 *)psDLParamDescriptorList[param].pValue) = *((U8 *)value);
            break;
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

/*give default values for all parameters in data layer. Currently set them all just 0.*/
void DL_setDefaultValuesForParameters(void)
{
    int defaultVal = 0;
    for (int i = 0; i < DLNumberOfParams; i++)
    {
        SetDataAccordingToType((DLParam)i, &defaultVal, psDLParamDescriptorList[((DLParam)i)].eType);
    }
}
