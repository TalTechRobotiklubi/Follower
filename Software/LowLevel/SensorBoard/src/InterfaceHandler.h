/*
 * InterfaceHandler.h
 *
 *  Created on: Jul 24, 2014
 *      Author: peeter
 */

#ifndef INTERFACEHANDLER_H_
#define INTERFACEHANDLER_H_

#include "InterfaceConfig.h"

#define INTERFACE_MSG_SIZE  32

typedef struct
{
	uint16_t id;
	uint16_t length;
	uint8_t data[INTERFACE_MSG_SIZE];
	PacketDescriptor* packet;
	int16_t period;
} InterfaceMessage;

void InterfaceHandler_transmitData(Interface interface, void (*funcToDriver)(InterfaceMessage* msg));
Bool InterfaceHandler_checkIfReceivedMessageExists(Interface interface, InterfaceMessage* msg);
void InterfaceHandler_storeReceivedData(InterfaceMessage* msg);

#endif /* INTERFACEHANDLER_H_ */
