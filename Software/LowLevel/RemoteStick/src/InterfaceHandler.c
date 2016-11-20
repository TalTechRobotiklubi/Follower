#include "InterfaceHandler.h"
#include "DataLayer.h"

void sendDataLayerDataToInterface(PacketDescriptor* packetDesc, InterfaceMessage* msg, void (*funcToDriver)(InterfaceMessage* msg));
uint8_t getBitmaskForMessage(uint8_t bitPosition, int16_t length);
void initalizeInterfaceMessage(InterfaceMessage *message, PacketDescriptor* packetDesc);

void InterfaceHandler_transmitData(Interface interface, void (*funcToDriver)(InterfaceMessage* msg))
{
	uint8_t i;
	InterfaceMessage message;

	NodeInterfaceDescriptor interfaceDesc = InterfaceList[interface];
	for (i = 0; i < interfaceDesc.transmitPacketCount; i++)
	{
		InterfaceTransmitPacket* transmitPacket = &interfaceDesc.transmitPacketList[i];

		// periodic packets
		if (transmitPacket->period >= 0)
		{
			if (transmitPacket->elapsed == 0)
			{
				initalizeInterfaceMessage(&message, transmitPacket->packet);
				sendDataLayerDataToInterface(transmitPacket->packet, &message, funcToDriver);
				transmitPacket->elapsed = transmitPacket->period;
			}
		}
		else // aperiodic packets
		{
			if (transmitPacket->period == PACKET_NEW)
			{
				initalizeInterfaceMessage(&message, transmitPacket->packet);
				sendDataLayerDataToInterface(transmitPacket->packet, &message, funcToDriver);
				DL_setDataInvalid(transmitPacket->packet);
				transmitPacket->period = PACKET_WAITING;
			}
		}
	}
}

/* NB! May be called from interrupt!!! Function not re-entrant, so disable interrupt(s) when calling it from main*/
Bool InterfaceHandler_checkIfReceivedMessageExists(InterfaceMessage* msg)
{
	uint8_t i;

	NodeInterfaceDescriptor interfaceDesc = InterfaceList[msg->interface];
	for (i = 0; i < interfaceDesc.receivePacketCount; i++)
	{
		const InterfaceReceivePacket* receivePacket = &interfaceDesc.receivePacketList[i];
		PacketDescriptor* packetDesc = receivePacket->packet;

		if (packetDesc->id == msg->id && packetDesc->dlc == msg->length)
		{
			msg->packet = receivePacket->packet;
			msg->period = receivePacket->period;
			return TRUE;
		}
	}
	return FALSE;
}

/* expects that InterfaceHandler_checkIfReceivedMessageExists is called first*/
void InterfaceHandler_storeReceivedData(InterfaceMessage* msg)
{
	PacketDescriptor* packetDesc = msg->packet;
	uint8_t byteIndex, bitPosition, j, type;
	int16_t length;
	uint8_t dataLayerOk;
	uint32_t data = 0;

	dataLayerOk = 0;

	/*go through all the parameters in the message and store them into data layer*/
	for (j = 0; j < packetDesc->parameterCount; j++)
	{
		byteIndex = ((packetDesc->parameterList + j)->startBit / 8);
		bitPosition = ((packetDesc->parameterList + j)->startBit % 8);
		length = (int16_t)((packetDesc->parameterList + j)->lengthBits);

		/*get parameter type from data layer*/
		type = DL_getDataType((packetDesc->parameterList + j)->param);

		switch(type)
		{
			case TypeBool:
			case TypeU2:
			case TypeS2:
			case TypeU4:
			case TypeS4:
			case TypeU8:
			case TypeS8:
				if (((bitPosition + length) <= 8) && ((bitPosition + length) >= 0))
				{
					/*involves only one byte */
					data = msg->data[byteIndex];
					/*sanity check*/
					data = (data >> (8 - bitPosition - length)) & (0xFF >> (8 - length));
					DL_setDataWithoutAffectingStatus((packetDesc->parameterList + j)->param, &data);
					dataLayerOk++;
				}
				break;
			case TypeU16:
			case TypeS16:
				/*sanity check*/
				if ((length <= 16) && (length > 8))
				{
					/*involves two bytes */
					/*first byte, bit position is assumed to be 0 and the byte is fully for this parameter*/
					data = (msg->data[byteIndex] << 8) & 0xFF00;
					length = length - 8;
					/*second byte, bit position is still 0, shift if length is not full byte*/
					data |= (msg->data[byteIndex + 1] & 0xFF);
					data = (uint16_t)(data >> (8 - length));
					DL_setDataWithoutAffectingStatus((packetDesc->parameterList + j)->param, &data);
					dataLayerOk++;
				}
				break;
			case TypeU32:
			case TypeS32:
			case TypeFloat:
				/*sanity check*/
				if ((length <= 32) && (length > 24))
				{
					/*involves 4 bytes */
					/*first 3 bytes, bit position is assumed to be 0 and the bytes are fully for this parameter*/
					data = (msg->data[byteIndex] << 24) & 0xFF000000;
					data += (msg->data[byteIndex + 1] << 16) & 0xFF0000;
					data += (msg->data[byteIndex + 2] << 8) & 0xFF00;
					length = length - 24;
					/*fourth byte, bit position is still 0, shift if length is not full byte*/
					data |= (msg->data[byteIndex + 3] & 0xFF);
					data = (uint32_t)(data >> (8 - length));
					DL_setDataWithoutAffectingStatus((packetDesc->parameterList + j)->param, &data);
					dataLayerOk++;
				}
				break;
			default:
				break;
		}
	}
	if (dataLayerOk == packetDesc->parameterCount)
	{
		NodeInterfaceDescriptor* interface = &InterfaceList[msg->interface];
		for (j = 0; j < interface->receivePacketCount; j++)
		{
			InterfaceReceivePacket* receivePacket = &interface->receivePacketList[j];
			if (receivePacket->packet == packetDesc)
			{
				if (receivePacket->period < 0)
					receivePacket->period = PACKET_NEW;
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

	for (j = 0; j < packetDesc->parameterCount; j++)
	{
		byteIndex = ((packetDesc->parameterList + j)->startBit / 8);
		bitPosition = ((packetDesc->parameterList + j)->startBit % 8);
		length = (int16_t)((packetDesc->parameterList + j)->lengthBits);
		/*get parameter type from data layer*/
		type = DL_getDataType((packetDesc->parameterList + j)->param);

		switch(type)
		{
			case TypeBool:
			case TypeU2:
			case TypeS2:
			case TypeU4:
			case TypeS4:
			case TypeU8:
			case TypeS8:
				/*involves only one byte */
				bitmask = getBitmaskForMessage(bitPosition, length);
				DL_peekData((packetDesc->parameterList + j)->param, &data);
				message->data[byteIndex] |= (((((uint8_t)data) & 0xFF) << (8 - length - bitPosition)) & bitmask);
				break;
			case TypeU16:
			case TypeS16:
				/*involves two bytes */
				DL_peekData((packetDesc->parameterList + j)->param, &data);
				/*first byte, bit position is assumed to be 0 and the byte is fully for this parameter*/
				message->data[byteIndex] |= (uint8_t)((data >> (length - 8)) & 0xFF);
				/*second byte, bit position is still 0*/
				bitmask = getBitmaskForMessage(0, length - 8);
				message->data[byteIndex + 1] |= (uint8_t)(((data & 0xFF) << (16 - length)) & bitmask);
				break;
			case TypeU32:
			case TypeS32:
			case TypeFloat:
				/*involves four bytes */
				DL_peekData((packetDesc->parameterList + j)->param, &data);
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
	message->id = packetDesc->id;
	message->length = packetDesc->dlc;

	int i;
	for (i = 0; i < message->length; i++)
	{
		message->data[i] = 0;
	}
}

