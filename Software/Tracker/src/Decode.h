#pragma once

#include "image.h"

struct Decoder;

Decoder* DecoderCreate();
bool DecodeFrame(Decoder* dc, const uint8_t* data, int len, int targetWidth,
                 int targetHeight, rgba_image* out);
