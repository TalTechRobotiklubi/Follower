#pragma once

#include <stdint.h>
#include "IoVec.h"

struct Encoder;

Encoder* EncoderCreate(int w, int h);

IoVec EncodeImage(Encoder* encoder, const uint8_t* raw, uint8_t* activeMap);
