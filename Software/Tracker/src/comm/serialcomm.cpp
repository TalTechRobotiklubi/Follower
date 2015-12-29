#include "SerialComm.h"

#include <assert.h>
#include <cstdio>
#include <string>

#include "datalayer.h"

// splitted u16 type, which can be accessed in separate byte manner
typedef union
{
    struct
    {
        uint8_t byteLow;
        uint8_t byteHigh;
    } u8;
    uint16_t u16;
} splitU16;

// UART-CAN message format
// +---------+--------+---------------------+---------+---------+-----+----------+----------+----------+
// | Byte 1  | Byte 2 | Byte 3              | Byte 4  | Byte 5  | ... | Byte 3+N | Byte 4+N | Byte 5+N |
// +---------+--------+---------------------+---------+---------+-----+----------+----------+----------+
// | 0xAA    | Msg ID | length (n), n = 1-8 | Data[0] | Data[1] | ... | Data[N]  | Checksum | Checksum |
// +---------+--------+---------------------+---------+---------+-----+----------+----------+----------+

// For UART_CAN message parsing a local state machine is used
enum MessageAnalyseState
{
    noData,
    preambulaFound,
    idOK,
    lengthOK,
    crcFirstbyte,
    crcCheck,
    messageOk
};

static void sendMessage(InterfaceMessage* msg);

SerialComm::SerialComm(void)
{
	DL_init();
}

void SerialComm::parseMessages(const char* buffer, size_t size)
{
    static MessageAnalyseState analyseState = noData;
	static InterfaceMessage message;
    static uint16_t length;
    static uint16_t dataIndex;
    static splitU16 messageCrc;
	static uint16_t calcCrc;

    // go through all the buffer, search valid messages
    for(unsigned int i = 0; i < size; i++)
    {
		switch (analyseState)
		{
		case noData:
			// search preambula '0xAA'
			if (buffer[i] == 0xaa || buffer[i] == 0xAA)
			{
				// clear message and variables for next data
				clearMessageStorage(message);
				length = 0;
				dataIndex = 0;
				messageCrc.u16 = 0;
				// preambula is part of CRC
				calcCrc = buffer[i];
				analyseState = preambulaFound;
			}
			break;
		case preambulaFound:
			message.id = buffer[i];
			calcCrc += message.id;
			analyseState = idOK;
			break;
		case idOK:
			message.length = buffer[i];
			if (InterfaceHandler_checkIfReceivedMessageExists(InterfaceUART, &message))
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
			messageCrc.u8.byteHigh = buffer[i];
			analyseState = crcCheck;
			break;
		case crcCheck:
			// store 2nd byte
			messageCrc.u8.byteLow = buffer[i];
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

// clears the element from UART_CAN Rx message storage
void SerialComm::clearMessageStorage(InterfaceMessage& message)
{
    message.id = 0;
    message.length = 0;
	for (int i = 0; i < INTERFACE_MSG_SIZE; ++i)
        message.data[i] = 0;
}

// send data to serial port
void SerialComm::sendControllerCommands()
{
	InterfaceHandler_transmitData(InterfaceUART, sendMessage);
}

void sendMessage(InterfaceMessage* msg)
{

}