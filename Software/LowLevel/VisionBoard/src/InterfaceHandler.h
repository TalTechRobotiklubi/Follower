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
	uint8_t length;
	uint8_t data[20];
	Packet packet;
} InterfaceMessage;

void InterfaceHandler_transmitData(Interface interface, void (*funcToDriver)(InterfaceMessage* msg), int16_t elapsedTime);

#endif /* INTERFACEHANDLER_H_ */
