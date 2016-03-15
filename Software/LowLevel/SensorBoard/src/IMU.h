/*
 * IMU.h
 *
 *  Created on: Mar 1, 2016
 *      Author: peeter
 */

#ifndef IMU_H_
#define IMU_H_

void IMU_init();
void IMU_task();

//Interrupt handler
void UART4_IRQHandler(void);

#endif /* IMU_H_ */
