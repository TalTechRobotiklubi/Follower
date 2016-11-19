#pragma once

#include "Image.h"

struct Decoder;

Decoder* DecoderCreate();
bool DecodeFrame(Decoder* dc, const uint8_t* data, int len, int targetWidth,
                 int targetHeight, RgbaImage* out);
