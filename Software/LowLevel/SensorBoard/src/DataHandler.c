#include "DataHandler.h"
#include "TaskHandler.h"
#include "InterfaceConfig.h"

/*Increase periodic transmit packet elapse time, if reached to period then it is notification
 *for interface modules (UART, CAN) to send packets out. Next time coming here it is over the period
 *so reset the counting by adding 1 count of elapsed time.
 *It means that packet periods must divide with period of DataHandler_TASK (5 ms), otherwise periodic packet
 *is never sent out. Another restriction is that interface modules (CAN, UART) must be called with same
 *period as DataHandler_TASK. */
void DataHandler_task(void)
{
	int i;
	for (i = 0; i < NumberOfInterfaces; ++i)
	{
		NodeInterfaceDescriptor interfaceDesc = psInterfaceList[i];
		int j;
		for (j = 0; j < interfaceDesc.uiTransmitPacketCount; j++)
		{
			InterfaceTransmitPacket transmitPacket = interfaceDesc.psTransmitPacketList[j];
			PacketDescriptor* packetDesc = &psPacketDescriptorList[transmitPacket.ePacket];

			// only periodic packets
			if (transmitPacket.ulPeriod > 0 && packetDesc->iPeriod >= 0)
			{
				uint16_t elapsedTime = TaskHandler_tableOfTasks[TASK_DATAHANDLER].period;
				packetDesc->iPeriod += elapsedTime;
				if (packetDesc->iPeriod > transmitPacket.ulPeriod)
					packetDesc->iPeriod = elapsedTime;
			}
		}
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

