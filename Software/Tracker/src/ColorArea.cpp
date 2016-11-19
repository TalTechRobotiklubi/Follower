#include "ColorArea.h"
#include "KinectFrame.h"
#include <string.h>
#include <libyuv.h>
#include "Constants.h"
#include <stdio.h>

void ColorAreaInit(ColorArea* area) {
	rgba_image_init(&area->image, kCandidateWidth, kCandidateHeight);
	memset(area->histogram, 0, sizeof(area->histogram));
}

void ColorAreaFill(ColorArea* area, const KinectFrame* frame, int x, int y, int w, int h) {
	const int stride = frame->rgbaWidth * 4;
	int offset = y * stride + x * 4;
	libyuv::ARGBScale(&frame->rgbaData[offset], stride, w, h,
		area->image.data, area->image.pitch,
		area->image.width, area->image.height,
		libyuv::kFilterNone
	);

	memset(area->histogram, 0, sizeof(area->histogram));

	for (int i = 0; i < area->image.bytes; i += 4) {
		uint8_t r = area->image.data[i];
		uint8_t g = area->image.data[i + 1];
		uint8_t b = area->image.data[i + 2];
		area->histogram[0][r] += 1.f;
		area->histogram[1][g] += 1.f;
		area->histogram[2][b] += 1.f;
	}
}