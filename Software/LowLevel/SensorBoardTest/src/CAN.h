#ifndef CAN_H_
#define CAN_H_

#include "InterfaceHandler.h"

void CAN_init(void);
void CAN_task(void);
uint8_t CAN_sentMessageCount();
InterfaceMessage* CAN_sentMessages();

#endif /*CAN_H_*/
