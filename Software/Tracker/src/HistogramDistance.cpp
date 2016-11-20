#include "HistogramDistance.h"
#include <math.h>
#include <numeric>

float HistogramDistance(const float* a, const float* b, int length) {
	const float count = float(length);
	const float mean1 = std::accumulate(a, a + length, 0.f) / count;
	const float mean2 = std::accumulate(b, b + length, 0.f) / count;

	float result = 0.f;
	for (int i = 0; i < length; i++) {
		result += sqrtf(a[i] * b[i]);
	}

	const float c = 1.f / sqrtf(mean1 * mean2 * count * count);
	return sqrtf(1.f - c * result);
}