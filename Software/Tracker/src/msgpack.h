#ifndef __MSGPACK_H__
#define __MSGPACK_H__

#include <cstdint>
#include <cstddef>

#define MSG_HEADER 0xAA

#define BODYPOS_MSG_LENGTH 13
#define DEPTH_FIELD_LENGTH 37

#define CAMERA_MSG_LENGTH 2

typedef enum { MT_BODY_POS = 0xC0, MT_DEPTH_FIELD = 0xC1 } msg_type;

void fill_bodypos_message(float x1, float y1, float x2, float y2,
                          uint8_t* dest);
void fill_depth_field_message(const uint16_t* depth_field, size_t len,
                              uint8_t* dest);

void fill_camera_message(int8_t hdeg, int8_t vdeg, uint8_t* dest);

#endif
