// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "PacketHandler.h"
#include "DataLayerCAN.h"

DataLayerCAN::DataLayerCAN(void)
{
    for (int i = 0; i < DLNumberOfParams; ++i)
        pbValidFlags_[i] = false;
}

DataLayerCAN::~DataLayerCAN(void)
{
}

// ----------------------------------------------------------------------------
// Use this function in logic layer to get data from DL. Handle also status for asynchronous data,
// so that after reading async data the status changes to WAITING.
// Param as parameter
// pValue as pointer to place where data will be stored from DL
// Returns if the value is valid or not
// ----------------------------------------------------------------------------
bool DataLayerCAN::DL_getData(DLParam param, DLValuePointer pValue)
{
	uint8_t numOfPackets, i, j;
	PacketWithIndex *packet;

    QMutexLocker locker(&mutex_);

	GetDataAccordingToType(param, pValue, psDLParamDescriptorList[param].eType);

	/*check if parameter is part of any async UART packets*/
	numOfPackets = packetHandler_.Packet_getNumOfUARTRxPackets();
	packet = packetHandler_.Packet_getUARTRxPacketsData();

	for (i = 0; i < numOfPackets; i++)
	{
		if ((packet[i].iperiod < 0) && (packetHandler_.Packet_getMessagePeriod(packet[i].index) < 0))
		{
			for (j = 0; j < packetHandler_.Packet_getMessageParameterCount(packet[i].index); j++)
			{
        if ((packetHandler_.Packet_getMessageParameterList(packet[i].index) + j)->param == param)
				{
					if (packet[i].iperiod != PACKET_WAITING)
					{
						packet[i].iperiod = PACKET_WAITING;
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

/*Use this function in COMM driver layer (CAN, UART etc.) to get data from DL.
 *Param as parameter
 *pValue as pointer to place where data will be stored from DL
 *Returns if the value is valid or not*/
bool DataLayerCAN::DL_getDataByComm(DLParam param, DLValuePointer pValue)
{
	GetDataAccordingToType(param, pValue, psDLParamDescriptorList[param].eType);

	return pbValidFlags_[param];
}

/*Use this function in logic layer to set data to DL. Handles status for asynchronous data
 *Param as parameter
 *pValue as pointer to place where data will be stored from DL*/
void DataLayerCAN::DL_setData(DLParam param, DLValuePointer pValue)
{
	uint32_t tempData, size, newValue;
    uint32_t mask = 0;
	uint8_t numOfPackets, i, j;
	PacketWithIndex *packet;
    Type type;

    QMutexLocker locker(&mutex_);

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
		SetDataAccordingToType(param, pValue, psDLParamDescriptorList[param].eType);

		/*check if parameter is part of any async UART packets*/
		numOfPackets = packetHandler_.Packet_getNumOfUARTTxPackets();
		packet = packetHandler_.Packet_getUARTTxPacketsData();

		for (i = 0; i < numOfPackets; i++)
		{
			if ((packet[i].iperiod < 0) && (packetHandler_.Packet_getMessagePeriod(packet[i].index) < 0))
			{
				for (j = 0; j < packetHandler_.Packet_getMessageParameterCount(packet[i].index); j++)
				{
          if ((packetHandler_.Packet_getMessageParameterList(packet[i].index) + j)->param == param)
					{
						packet[i].iperiod = PACKET_READY_TO_SEND;
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
void DataLayerCAN::DL_setDataByComm(DLParam param, DLValuePointer value)
{
	SetDataAccordingToType(param, value, psDLParamDescriptorList[param].eType);
	
	pbValidFlags_[param] = true;
}

Type DataLayerCAN::DL_getDataType(DLParam param)
{
	return psDLParamDescriptorList[param].eType;
}

void DataLayerCAN::DL_setDataValidity(DLParam param, bool validity)
{
	pbValidFlags_[param] = validity;
}

/*gets data from data layer*/
void DataLayerCAN::GetDataAccordingToType(DLParam param, DLValuePointer value, Type type)
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
void DataLayerCAN::SetDataAccordingToType(DLParam param, DLValuePointer value, Type type)
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
void DataLayerCAN::DL_setDefaultValuesForParameters(void)
{
    int defaultVal = 0;
    for (int i = 0; i < DLNumberOfParams; i++)
    {
        SetDataAccordingToType((DLParam)i, &defaultVal, psDLParamDescriptorList[((DLParam)i)].eType);
    }
}


QMutex& DataLayerCAN::getMutex()
{
    return mutex_; 
}
