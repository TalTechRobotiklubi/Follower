/*
 * CAN.h
 *
 *  Created on: Apr 14, 2012
 *      Author: Robotiklubi
 */

#ifndef CAN_H_
#define CAN_H_

#include "stdint.h"

extern void    CAN_init();
extern void    CAN_task(void);
/*interrupt handler*/
//void CAN1_RX0_IRQn(void);

#endif /* CAN_H_ */
