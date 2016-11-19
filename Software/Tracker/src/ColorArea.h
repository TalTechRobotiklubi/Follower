#pragma once
#include <stdint.h>
#include "image.h"

struct KinectFrame;

struct ColorArea {
	rgba_image image;
	float histogram[3][256];
};

void ColorAreaInit(ColorArea* area);
void ColorAreaFill(ColorArea* area, const KinectFrame* frame, int x, int y, int w, int h);