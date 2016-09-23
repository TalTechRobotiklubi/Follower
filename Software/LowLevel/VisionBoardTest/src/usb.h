#ifndef USB_H_
#define USB_H_

#include "InterfaceHandler.h"

void USB_init();
void USB_task();
uint8_t USB_sentMessageCount();
InterfaceMessage* USB_sentMessages();

void USB_setRobotConstrolMessagePending(int16_t translationSpeed, int16_t rotationSpeed);

#endif /* USB_H_ */
