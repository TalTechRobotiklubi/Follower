#ifndef CAN_H_
#define CAN_H_

#include "InterfaceHandler.h"

void CAN_init(void);
void CAN_task(void);

void CAN_setDistanceMessagePending(uint8_t dist1, uint8_t dist2);
uint8_t CAN_sentMessageCount();
InterfaceMessage* CAN_sentMessages();

#endif /*CAN_H_*/
