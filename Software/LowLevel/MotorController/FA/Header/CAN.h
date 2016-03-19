#ifndef _CAN_H_
#define _CAN_H_

#include "stm32f10x.h"

void CAN_init(void);
void CAN_task();
void USB_LP_CAN1_RX0_IRQHandler(void);
#endif // _CAN_H_
