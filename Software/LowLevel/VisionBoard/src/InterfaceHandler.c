#include "InterfaceHandler.h"
#include "DataLayer.h"

void sendDataLayerDataToInterface(PacketDescriptor* packetDesc, InterfaceMessage* msg, void (*funcToDriver)(InterfaceMessage* msg));
uint8_t getBitmaskForMessage(uint8_t bitPosition, int16_t length);
void initalizeInterfaceMessage(InterfaceMessage *message, PacketDescriptor* packetDesc);

void InterfaceHandler_transmitData(Interface interface, void (*funcToDriver)(InterfaceMessage* msg), int16_t elapsedTime)
{
	uint8_t i;
	InterfaceMessage message;

	NodeInterfaceDescriptor interDesc = psInterfaceList[interface];
	for (i = 0; i < interDesc.uiTransmitPacketCount; i++)
	{
		InterfaceTransmitPacket transmitPacket = interDesc.psTransmitPacketList[i];
		PacketDescriptor packetDesc = psPacketDescriptorList[transmitPacket.ePacket];

		// only periodic packets
		if (transmitPacket.ulPeriod > 0)
		{
			packetDesc.iPeriod += elapsedTime;
			if (packetDesc.iPeriod >= transmitPacket.ulPeriod)
			{
				packetDesc.iPeriod = 0;
				message.packet = transmitPacket.ePacket;
				initalizeInterfaceMessage(&message, &packetDesc);
				sendDataLayerDataToInterface(&packetDesc, &message, funcToDriver);
			}
		}
	}
}


/*gets data from DL and stores into interface message*/
void sendDataLayerDataToInterface(PacketDescriptor* packetDesc, InterfaceMessage* message, void (*funcToDriver)(InterfaceMessage* msg))
{
	uint8_t byteIndex, bitPosition;
	uint16_t j;
	int16_t length;
	uint8_t bitmask;
	Type type;
	uint32_t data;

	for (j = 0; j < packetDesc->uiParameterCount; j++)
	{
		byteIndex = ((packetDesc->psParameterList + j)->uiStartBit / 8);
		bitPosition = ((packetDesc->psParameterList + j)->uiStartBit % 8);
		length = (int16_t)((packetDesc->psParameterList + j)->uiLengthBits);
		/*get parameter type from data layer*/
		type = DL_getDataType((packetDesc->psParameterList + j)->eParam);

		switch(type)
		{
			case TypeBool:
			case TypeU4:
			case TypeS4:
			case TypeU8:
			case TypeS8:
				/*involves only one byte */
				bitmask = getBitmaskForMessage(bitPosition, length);
				DL_getDataByComm((packetDesc->psParameterList + j)->eParam, &data);
				message->data[byteIndex] |= (((((uint8_t)data) & 0xFF) << (8 - length - bitPosition)) & bitmask);
				break;
			case TypeU16:
			case TypeS16:
				/*involves two bytes */
				DL_getDataByComm((packetDesc->psParameterList + j)->eParam, &data);
				/*first byte, bit position is assumed to be 0 and the byte is fully for this parameter*/
				message->data[byteIndex] |= (uint8_t)((data >> (length - 8)) & 0xFF);
				/*second byte, bit position is still 0*/
				bitmask = getBitmaskForMessage(0, length - 8);
				message->data[byteIndex + 1] |= (uint8_t)(((data & 0xFF) << (16 - length)) & bitmask);
				break;
			case TypeU32:
			case TypeS32:
				/*involves four bytes */
				DL_getDataByComm((packetDesc->psParameterList + j)->eParam, &data);
				/*first 3 bytes, bit position is assumed to be 0 and the bytes are fully for this parameter*/
				message->data[byteIndex] |= (uint8_t)((data >> (length - 8)) & 0xFF);
				message->data[byteIndex + 1] |= (uint8_t)((data >> (length - 16)) & 0xFF);
				message->data[byteIndex + 2] |= (uint8_t)((data >> (length - 24)) & 0xFF);
				/*fourth byte, bit position is still 0*/
				bitmask = getBitmaskForMessage(0, length - 8);
				message->data[byteIndex + 3] |= (uint8_t)(((data & 0xFF) << (32 - length)) & bitmask);
				break;
			default:
				break;
		}
	}

	funcToDriver(message);
}


uint8_t getBitmaskForMessage(uint8_t bitPosition, int16_t length)
{
  uint8_t mask = 0xFF;
  uint8_t zerosFromRight, i;

  // shift to left so upper bits are set to zero
  mask = mask >> bitPosition;

  if (length > 0)
  {
	  // check how many bits should be set to 0 from right side
	  if (length > 8) length = 8;
	  zerosFromRight = (8 - length - bitPosition);
	  for (i = 0; i < zerosFromRight; i++)
	  {
		  mask = mask & ~(1 << i);
	  }
	  // invert the mask as bits not involved with data should be set to 1
  }
  else
  {
	  mask = 0;
  }
  return mask;
}


void initalizeInterfaceMessage(InterfaceMessage* message, PacketDescriptor* packetDesc)
{
	message->id = packetDesc->uiID;
	message->length = packetDesc->uiDLC;

	int i;
	for (i = 0; i < message->length; i++)
	{
		message->data[i] = 0;
	}
}

