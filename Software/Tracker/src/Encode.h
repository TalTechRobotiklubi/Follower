#pragma once

#include <stdint.h>
#include "IoVec.h"

struct Encoder;
struct ActiveMap;

Encoder* EncoderCreate(int w, int h);

IoVec EncodeImage(Encoder* encoder, const uint8_t* raw, const ActiveMap* map);
