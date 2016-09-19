#ifndef USB_H_
#define USB_H_

#include "InterfaceHandler.h"

void USB_init();
void USB_task();
uint8_t USB_sentMessageCount();
InterfaceMessage* USB_sentMessages();

#endif /* USB_H_ */
