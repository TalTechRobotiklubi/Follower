#ifndef USART_H_
#define USART_H_

#include "PacketConfig.h"

void USART_init(void);
void USART_task(void);

void USART_setPidSetupMessagePending(uint8_t param, float value, uint8_t updating);
void USART_setGyroMessagePending(int16_t yaw, int16_t pitch, int16_t roll);

#endif /*USART_H_*/
