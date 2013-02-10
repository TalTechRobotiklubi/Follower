/*
 * CAN.h
 *
 *  Created on: Apr 14, 2012
 *      Author: Robotiklubi
 */

#ifndef CAN_H_
#define CAN_H_

#include "stdint.h"


extern void    CAN_init(uint8_t remapped);
extern int16_t CAN_getRequestedSpeed(void);
extern void    CAN_TASK(void);
/*interrupt handler*/
void USB_LP_CAN1_RX0_IRQHandler(void);

#endif /* CAN_H_ */
