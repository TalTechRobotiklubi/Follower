// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "DataLayerCAN.h"
#include "InterfaceConfig.h"
#include "DataLayerConfig.h"

DataLayerCAN::DataLayerCAN(void)
{
    for (int i = 0; i < DLNumberOfParams; ++i)
        pbValidFlags_[i] = false;
}

DataLayerCAN::~DataLayerCAN(void)
{
}

/*Decrease periodic transmit packet elapse time, if reached to 0 then it is notification
 *for interface modules (UART, CAN) to send packets out and update the elapsed time to initial value.
 *
 *In case of async packet it is set to PACKET_WAITING if all parameters in the packet are invalid.
 */
void DataLayerCAN::DL_task(uint16_t timeElapsedMs)
{
  for (int i = 0; i < NumberOfInterfaces; ++i)
  {
    NodeInterfaceDescriptor interfaceDesc = InterfaceList[i];
    uint j;
    for (j = 0; j < interfaceDesc.transmitPacketCount; j++)
    {
      InterfaceTransmitPacket* transmitPacket = &interfaceDesc.transmitPacketList[j];

      // only periodic packets
      if (transmitPacket->period > 0)
      {
        transmitPacket->elapsed -= timeElapsedMs;
        if (transmitPacket->elapsed < 0)
          transmitPacket->elapsed = 0;
      }
    }
    for (j = 0; j < interfaceDesc.receivePacketCount; j++)
    {
      InterfaceReceivePacket receivePacket = interfaceDesc.receivePacketList[j];
      PacketDescriptor* packetDesc = &PacketDescriptorList[receivePacket.packet];
      bool allInvalid = false;

      // only async packets
      if (packetDesc->period < 0 && packetDesc->period == PACKET_NEW)
      {
        for (j = 0; j < packetDesc->parameterCount; j++)
          allInvalid |= pbValidFlags_[(packetDesc->parameterList + j)->param];
        if (!allInvalid)
          packetDesc->period = PACKET_WAITING;
      }
    }
  }
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
  QMutexLocker locker(&mutex_);
  bool retVal = false;

	GetDataAccordingToType(param, pValue, psDLParamDescriptorList[param].eType);

  /*check if parameter is part of any async packets*/
  for (int i = 0; i < NumberOfPackets; i++)
  {
    PacketDescriptor *packet = &PacketDescriptorList[i];
    if (packet->period < 0)
    {
      for (int j = 0; j < packet->parameterCount; j++)
      {
        if ((packet->parameterList + j)->param == param)
        {
          retVal = pbValidFlags_[param];
          if (pbValidFlags_[param] == true)
            pbValidFlags_[param] = false;
          return retVal;
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
bool DataLayerCAN::DL_getDataWithoutAffectingStatus(DLParam param, DLValuePointer pValue)
{
	GetDataAccordingToType(param, pValue, psDLParamDescriptorList[param].eType);

	return pbValidFlags_[param];
}

/*Use this function in logic layer to set data to DL. Handles status for asynchronous data
 *Param as parameter
 *pValue as pointer to place where data will be stored from DL*/
void DataLayerCAN::DL_setData(DLParam param, DLValuePointer pValue)
{
  uint32_t tempData, newValue;
  uint32_t mask = 0;
  uint8_t i, j;
  Type type;
  uint32_t size;

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
    SetDataAccordingToType(param, pValue, type);

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
void DataLayerCAN::DL_setDataWithoutAffectingStatus(DLParam param, DLValuePointer value)
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

void DataLayerCAN::DL_setAsyncPacketInvalid(PacketDescriptor* packetDesc)
{
  if (packetDesc->period < 0)
  {
    int j;
    for (j = 0; j < packetDesc->parameterCount; j++)
      pbValidFlags_[(packetDesc->parameterList + j)->param] = false;
    packetDesc->period = PACKET_WAITING;
  }
}

QMutex& DataLayerCAN::getMutex()
{
    return mutex_; 
}
