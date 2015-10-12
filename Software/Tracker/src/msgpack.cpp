#include "msgpack.h"
#include <string.h>
#include "crc.h"
#include <math.h>
#include <stdio.h>

static float clampf(float v, float low, float high) {
	if (v < low) {
		return low;
	}
	else if (v > high) {
		return high;
	}

	return v;
}

uint16_t swap16(uint16_t v) {
	return ((v & 0xFF) << 8) | ((v & 0xFF00) >> 8);
}

void fill_bodypos_message(float x1, float y1, float x2, float y2, uint8_t* dest) {

	uint16_t payload[4] = {
		(uint16_t)clampf(x1, 0.0f, 512.f),
		(uint16_t)clampf(y1, 0.0f, 424.f),
		(uint16_t)clampf(x2, 0.0f, 512.f),
		(uint16_t)clampf(y2, 0.0f, 424.f)
	};

	for (auto& v : payload) {
		v = swap16(v);
	}

	dest[0] = MSG_HEADER;
	dest[1] = MT_BODY_POS;
	dest[2] = 8; // 4 * u16 payload

	memcpy(&dest[3], payload, sizeof(payload));

	uint16_t crc = crc16_sum(dest, BODYPOS_MSG_LENGTH - sizeof(uint16_t));

	dest[11] = (uint8_t)((crc >> 8) & 0xff);
	dest[12] = (uint8_t)crc;

}

void fill_depth_field_message(const uint16_t* depth_field, size_t len, uint8_t* dest) {

	dest[0] = MSG_HEADER;
	dest[1] = MT_DEPTH_FIELD;

	size_t payload_length = len * sizeof(uint16_t);

	dest[2] = (uint8_t)payload_length;
	
	memcpy(&dest[3], depth_field, payload_length);

	uint16_t crc = crc16_sum(dest, DEPTH_FIELD_LENGTH - sizeof(uint16_t));

	uint8_t* crc_begin = &dest[3] + payload_length;
	crc_begin[0] = crc << 8;
	crc_begin[1] = (uint8_t)crc;
}