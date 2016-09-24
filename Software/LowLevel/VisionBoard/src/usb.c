/*
 * usb.c
 *
 *  Created on: Dec 10, 2015
 *      Author: peeter
 */

#include "usb.h"

#include "tm_stm32f4_usb_vcp.h"

#include "DataLayer.h"
#include "InterfaceHandler.h"

typedef union
{
	struct
	{
		uint8_t byteLow;
		uint8_t byteHigh;
	} u8;
	uint16_t u16;
} splitU16;

typedef enum
{
	noData,
	preambulaFound,
	idOK,
	lengthOK,
	crcFirstbyte,
	crcCheck,
	messageOk
}
MessageAnalyseState;

static void parseMessages(uint8_t* buffer, uint16_t size);
static void handleRecievedData();
static void clearMessageStorage(InterfaceMessage* message);
static void handleTransmitData(void);
static void sendMessage(InterfaceMessage* msg);

void USB_init()
{
	TM_USB_VCP_Init();
}

void USB_task()
{
	handleRecievedData();
	handleTransmitData();
}

void handleRecievedData()
{
	/* USB configured OK, drivers OK */
	if (TM_USB_VCP_GetStatus() == TM_USB_VCP_CONNECTED)
	{
		uint8_t buffer[100];
		int i = 0;
		/* If something arrived at VCP */
		while (TM_USB_VCP_Getc(&buffer[i]) == TM_USB_VCP_DATA_OK)
		{
			if (++i == 100)
				break;
		}
		if (i > 0)
			parseMessages(buffer, i);
	}
}

void parseMessages(uint8_t* buffer, uint16_t size)
{
    static MessageAnalyseState analyseState = noData;
	static InterfaceMessage message;
    static uint16_t length;
    static uint16_t dataIndex;
    static splitU16 messageCrc;
	static uint16_t calcCrc;
	uint16_t i;

    // go through all the buffer, search valid messages
    for(i = 0; i < size; i++)
    {
		switch (analyseState)
		{
		case noData:
			// search preambula '0xAA'
			if (buffer[i] == 0xaa || buffer[i] == 0xAA)
			{
				// clear message and variables for next data
				clearMessageStorage(&message);
				length = 0;
				dataIndex = 0;
				messageCrc.u16 = 0;
				// preambula is part of CRC
				calcCrc = buffer[i];
				analyseState = preambulaFound;
			}
			break;
		case preambulaFound:
			if (buffer[i] == 0xaa || buffer[i] == 0xAA)
				analyseState = preambulaFound;
			else
			{
				message.id = buffer[i];
				calcCrc += message.id;
				analyseState = idOK;
			}
			break;
		case idOK:
			message.length = buffer[i];
			message.interface = InterfaceUSB;
			__disable_irq();
			if (InterfaceHandler_checkIfReceivedMessageExists(&message))
			{
				calcCrc += message.length;
				length = message.length;
				analyseState = lengthOK;
			}
			else
			{
				// not valid message, continue with seaching next preambula
				analyseState = noData;
			}
			__enable_irq();
			break;
		case lengthOK:
			// copy data
			if (length != 0)
			{
				message.data[dataIndex] = buffer[i];
				calcCrc += buffer[i];
				dataIndex++;
				length--;
				if (length == 0)
				{
					// continue with CRC
					analyseState = crcFirstbyte;
				}
			}
			else
			{
				// length should not be 0 from the beginning -> logic mistake, start from the beginning
				analyseState = noData;
			}
			break;
		case crcFirstbyte:
			// store crc first byte
			messageCrc.u8.byteLow = buffer[i];
			analyseState = crcCheck;
			break;
		case crcCheck:
			// store 2nd byte
			messageCrc.u8.byteHigh = buffer[i];
			// check CRC
			if (calcCrc == messageCrc.u16)
			{
				// new valid message received, copy data into data layer
				InterfaceHandler_storeReceivedData(&message);
				// ready for next packet
				analyseState = noData;
			}
			else
			{
				analyseState = noData;
			}
			break;
		default:

			break;
		}
    }
}

void clearMessageStorage(InterfaceMessage* message)
{
	uint16_t i;
    message->id = 0;
    message->length = 0;
	for (i = 0; i < INTERFACE_MSG_SIZE; ++i)
        message->data[i] = 0;
}

void handleTransmitData(void)
{
	InterfaceHandler_transmitData(InterfaceUSB, sendMessage);
}

void sendMessage(InterfaceMessage* msg)
{
	splitU16 crc;
	uint16_t j;

	TM_USB_VCP_Putc(0xAA);
	crc.u16 = 0xAA;
	TM_USB_VCP_Putc(msg->id);
	crc.u16 += msg->id;
	TM_USB_VCP_Putc(msg->length);
	crc.u16 += msg->length;
	for (j = 0; j < msg->length; j++)
	{
		TM_USB_VCP_Putc(msg->data[j]);
		crc.u16 += msg->data[j];
	}
	TM_USB_VCP_Putc(crc.u8.byteLow);
	TM_USB_VCP_Putc(crc.u8.byteHigh);
}
