/*
 * test_global.h
 *
 *  Created on: May 22, 2016
 *      Author: peeter
 */

#ifndef TEST_GLOBAL_H_
#define TEST_GLOBAL_H_

#include <stdint.h>

typedef union
{
	float fl;
	uint8_t d[4];
} SplitFloat;

typedef union
{
	int16_t i;
	uint8_t d[2];
} SplitInt16;

typedef union
{
	uint16_t u;
	uint8_t d[2];
} SplitUint16;

extern uint8_t DataHandlerOffset;
extern uint8_t CanOffset;
extern uint8_t UsbOffset;

extern uint8_t DistanceSensorPacketTransmitIndex;
extern uint8_t QuaterionsPacketTransmitIndex;

#endif /* TEST_GLOBAL_H_ */
