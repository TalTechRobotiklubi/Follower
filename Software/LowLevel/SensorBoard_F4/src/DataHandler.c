#include "DataLayer.h"
#include "TaskHandler.h"

#if 0
void checkTimeouts(uint8_t numOfPackets, PacketWithIndex *packet);
void setDataReadyToSend(uint8_t numOfPackets, PacketWithIndex *packet);

/*Handles data timeouts and COMM module async data transfers*/
void DataHandler_TASK(void)
{
	uint8_t numOfPackets;
	PacketWithIndex *packet;

#if (ENABLE_CAN_INTERFACE == 1)
	/*check CAN timeouts*/
	numOfPackets = Packet_getNumOfCANRxPackets();
	packet = Packet_getCANRxPacketsData();
	checkTimeouts(numOfPackets, packet);
#endif

#if (ENABLE_UART_INTERFACE == 1)
	/*check UART timeouts*/
	numOfPackets = Packet_getNumOfUARTRxPackets();
	packet = Packet_getUARTRxPacketsData();
	checkTimeouts(numOfPackets, packet);
#endif
}

/*checks if any received data is timed out*/
void checkTimeouts(uint8_t numOfPackets, PacketWithIndex *packet)
{
#if 0
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
#endif
}
#endif

