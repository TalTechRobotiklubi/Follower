// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "DataLayer.h"
#include "DataLayerConfig.h"
#include "InterfaceConfig.h"
#include "TaskHandler.h"

// ----------------------------------------------------------------------------
// Variables
// ----------------------------------------------------------------------------
static Bool priv_validFlags[DLNumberOfParams];
// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
static void setDataAccordingToType(DLParam param, DLValuePointer value, Type type);
static void getDataAccordingToType(DLParam param, DLValuePointer value, Type type);
static void markNewAsyncMessageReadyForSending(DLParam param);
static void clearPacketsToDefault();

void DL_init()
{
	int i = 0;
	for (; i < DLNumberOfParams; ++i)
		priv_validFlags[i] = FALSE;
	clearPacketsToDefault();
}

void clearPacketsToDefault()
{
	int i;
	for (i = 0; i < NumberOfInterfaces; ++i)
	{
		NodeInterfaceDescriptor interfaceDesc = InterfaceList[i];
		int j;
		for (j = 0; j < interfaceDesc.transmitPacketCount; j++)
		{
			InterfaceTransmitPacket* transmitPacket = &interfaceDesc.transmitPacketList[j];
			if (transmitPacket->period > 0)
				transmitPacket->elapsed = transmitPacket->period;
			else
				transmitPacket->period = PACKET_WAITING;
		}
		for (j = 0; j < interfaceDesc.receivePacketCount; j++)
		{
			const InterfaceReceivePacket* receivePacket = &interfaceDesc.receivePacketList[j];
			PacketDescriptor* packetDesc = receivePacket->packet;
			if (packetDesc->period < 0)
				packetDesc->period = PACKET_WAITING;
		}
	}
}

/*Decrease periodic transmit packet elapse time, if reached to 0 then it is notification
 *for interface modules (UART, CAN) to send packets out and update the elapsed time to initial value.
 *
 *In case of async packet it is set to PACKET_WAITING if all parameters in the packet are invalid.
 */
void DL_task(void)
{
	int i;
	for (i = 0; i < NumberOfInterfaces; ++i)
	{
		NodeInterfaceDescriptor interfaceDesc = InterfaceList[i];
		int j;
		for (j = 0; j < interfaceDesc.transmitPacketCount; j++)
		{
			InterfaceTransmitPacket* transmitPacket = &interfaceDesc.transmitPacketList[j];

			// only periodic packets
			if (transmitPacket->period > 0)
			{
				uint16_t elapsedTime = TaskHandler_tableOfTasks[TASK_DATAHANDLER].period;
				transmitPacket->elapsed -= elapsedTime;
				if (transmitPacket->elapsed < 0)
					transmitPacket->elapsed = 0;
			}
		}
		for (j = 0; j < interfaceDesc.receivePacketCount; j++)
		{
			const InterfaceReceivePacket* receivePacket = &interfaceDesc.receivePacketList[j];
			PacketDescriptor* packetDesc = receivePacket->packet;
			Bool allInvalid = FALSE;

			// only new async receive packets
			if (packetDesc->period < 0 && packetDesc->period == PACKET_NEW)
			{
				int k;
				// update with new flag same package in all Tx interfaces
				for (k = 0; k < NumberOfInterfaces; ++k)
				{
					NodeInterfaceDescriptor interfaceDesc = InterfaceList[k];
					int l;
					for (l = 0; l < interfaceDesc.transmitPacketCount; l++)
					{
						InterfaceTransmitPacket* transmitPacket = &interfaceDesc.transmitPacketList[l];
						if (transmitPacket->packet == receivePacket->packet)
							transmitPacket->period = PACKET_NEW;
					}
				}

				for (k = 0; k < packetDesc->parameterCount; k++)
					allInvalid |= priv_validFlags[(packetDesc->parameterList + k)->param];
				if (!allInvalid)
					packetDesc->period = PACKET_WAITING;
			}
		}
	}
}

void DL_setDataInAsyncPacketInvalid(PacketDescriptor* packetDesc)
{
	if (packetDesc->period < 0)
	{
		int j;
		for (j = 0; j < packetDesc->parameterCount; j++)
			priv_validFlags[(packetDesc->parameterList + j)->param] = FALSE;
	}
}

#if 0
/*checks if any received data is timed out*/
void checkTimeouts(uint8_t numOfPackets, PacketWithIndex *packet)
{
	uint8_t i, j;

	for (i = 0; i < numOfPackets; i++)
	{
		/*only periodic packets*/
		if( ((packet[i].iperiod >= 0)) && (Packet_getMessagePeriod(packet[i].index >= 0)) )
		{
			/*increase timeout counter and check if it is exceeded 10 times its normal period*/
			packet[i].iperiod += TaskHandler_tableOfTasks[TASK_DATA].period;
			if (packet[i].iperiod >= (Packet_getMessagePeriod(packet[i].index) * 10))
			{
				/*set all packet parameter values as invalid*/
				for (j = 0; j < Packet_getMessageParameterCount(packet[i].index); j++)
				{
					DL_setDataValidity((Packet_getMessageParameterList(packet[i].index) + j)->eParam, FALSE);
				}
			}
		}
	}
}
#endif


// ----------------------------------------------------------------------------
// Use this function in logic layer to get data from DL.
// Param as parameter
// pValue as pointer to place where data will be stored from DL
// Returns if the value is valid or not. Validity of the value is determined in
// two ways: in case of periodic packet
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
	uint8_t i;
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
		markNewAsyncMessageReadyForSending(param);
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

void DL_setDataWithForcedAsyncSend(DLParam param, DLValuePointer value)
{
	setDataAccordingToType(param, value, psDLParamDescriptorList[param].eType);
	markNewAsyncMessageReadyForSending(param);
}

void markNewAsyncMessageReadyForSending(DLParam param)
{
	int i;
	for (i = 0; i < NumberOfInterfaces; ++i)
	{
		NodeInterfaceDescriptor interfaceDesc = InterfaceList[i];
		int j;
		for (j = 0; j < interfaceDesc.transmitPacketCount; j++)
		{
			InterfaceTransmitPacket* transmitPacket = &interfaceDesc.transmitPacketList[j];
			uint8_t packetFound = 0;

			// only async packets
			if (transmitPacket->period < 0)
			{
				PacketDescriptor* packet = transmitPacket->packet;
				int k;
				for (k = 0; k < packet->parameterCount; k++)
				{
					if ((packet->parameterList + j)->param == param)
					{
						transmitPacket->period = PACKET_NEW;
						packetFound = 1;
					}
				}
			}
			if (packetFound)
				break;
		}
	}
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
