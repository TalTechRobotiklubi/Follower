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

