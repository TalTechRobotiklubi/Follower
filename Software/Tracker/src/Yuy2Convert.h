#pragma once
#include <stddef.h>
#include <stdint.h>

struct Yuy2Convert;

Yuy2Convert* Yuy2ConvertCreate(int inputWidth, int inputHeight, int outputWidth,
                               int outputHeight);

uint8_t* Yuy2ToRgba(Yuy2Convert* converter, const uint8_t* yuy2);
void Yuy2ConvertDestroy(Yuy2Convert* converter);
void Yuy2SetQuality(Yuy2Convert* converter, int quality);
