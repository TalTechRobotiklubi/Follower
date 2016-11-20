#pragma once

#ifdef _WIN32
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __attribute__((visibility("default")))
#endif

extern "C" {
	DLL_EXPORT float HistogramDistance(const float* a, const float* b, int length);
}