/*
 * InterfaceHandler.h
 *
 *  Created on: Jul 24, 2014
 *      Author: peeter
 */

#ifndef INTERFACEHANDLER_H_
#define INTERFACEHANDLER_H_

#include "InterfaceConfig.h"

typedef struct
{
	uint16_t id;
	uint16_t length;
	uint8_t data[32];
	Packet packet;
	int16_t period;
} InterfaceMessage;

extern void InterfaceHandler_transmitData(Interface interface, void (*funcToDriver)(InterfaceMessage* msg));
extern void InterfaceHandler_transmitAsyncDataWithoutAffectingStatus(Interface interface, void (*funcToDriver)(InterfaceMessage* msg));
extern Bool InterfaceHandler_checkIfReceivedMessageExists(Interface interface, InterfaceMessage* msg);
extern void InterfaceHandler_storeReceivedData(InterfaceMessage* msg);

#endif /* INTERFACEHANDLER_H_ */
