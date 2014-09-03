
#ifndef VISION_H_
#define VISION_H_

#include <stdint.h>

typedef struct
{
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
}
VisionObject;

extern void Vision_init();
extern VisionObject* Vision_getObjects();
extern uint8_t Vision_numOfFoundObjects();
extern void Vision_TASK();
#endif /* VISION_H_ */
