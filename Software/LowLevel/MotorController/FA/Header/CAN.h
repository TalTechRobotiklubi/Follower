#ifndef _CAN_H_
#define _CAN_H_

#include "stm32f10x.h"

//typedef struct
//{
//
//	uint8_t*			pu8ControllerID;
//    uint8_t*            pu8MotorDirection;
//	float*				pfMotorSpeed;
//	float*				pfMotorCurrent;
//	float*				pfMotorTemp;
//	uint8_t*			pu8Fault;
//	float				fCanSpeedReference;
//}T_CAN;

void CAN_init(void);
void CAN_task();
void USB_LP_CAN1_RX0_IRQHandler(void);
#endif // _CAN_H_
