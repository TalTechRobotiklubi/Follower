/*
 * InterfaceHandler.h
 *
 *  Created on: Jul 24, 2014
 *      Author: peeter
 */

#ifndef INTERFACEHANDLER_H_
#define INTERFACEHANDLER_H_

#include <functional>

#include "InterfaceConfig.h"

#define DATA_BUFFER_SIZE 32

class DataLayerCAN;

typedef struct
{
	uint16_t id;
	uint16_t length;
  uint8_t data[DATA_BUFFER_SIZE];
	Packet packet;
	int16_t period;
} InterfaceMessage;

bool InterfaceHandler_checkIfReceivedMessageExists(Interface interface, InterfaceMessage* msg);
void InterfaceHandler_transmitData(DataLayerCAN* dataLayer, Interface interface,
                                   std::function<void(InterfaceMessage*)> sendFunction);
void InterfaceHandler_storeReceivedData(DataLayerCAN* dataLayer, InterfaceMessage* msg);

#endif /* INTERFACEHANDLER_H_ */
