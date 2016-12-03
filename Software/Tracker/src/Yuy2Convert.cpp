#include "Yuy2Convert.h"
#include <libyuv.h>
#include <stdlib.h>
#include "FlMath.h"

struct Yuy2Convert {
  int outputWidth;
  int outputHeight;
  int outputStride;
  int inputWidth;
  int inputHeight;
  int inputStride;
  int quality;

  size_t rgbaBufferLength;
  uint8_t* rgbaBuffer;

  size_t i420BufferLength;
  uint8_t* i420Buffer;

  size_t i420DownscaleBufferLength;
  uint8_t* i420DownscaleBuffer;
};

Yuy2Convert* Yuy2Convert_create(int inputWidth, int inputHeight,
                                int outputWidth, int outputHeight) {
  Yuy2Convert* converter = (Yuy2Convert*)malloc(sizeof(Yuy2Convert));
  converter->outputWidth = outputWidth;
  converter->outputHeight = outputHeight;
  converter->outputStride = outputWidth * 4;
  converter->inputWidth = inputWidth;
  converter->inputHeight = inputHeight;
  converter->inputStride = inputWidth * 2;
  converter->quality = 0;

  converter->rgbaBufferLength = outputWidth * outputHeight * 4;
  converter->rgbaBuffer = (uint8_t*)malloc(converter->rgbaBufferLength);

  converter->i420BufferLength = inputWidth * inputHeight * 3;
  converter->i420Buffer = (uint8_t*)malloc(converter->i420BufferLength);

  converter->i420DownscaleBufferLength = outputWidth * outputHeight * 3;
  converter->i420DownscaleBuffer =
      (uint8_t*)malloc(converter->i420DownscaleBufferLength);

  return converter;
}

uint8_t* Yuy2ToRgba(Yuy2Convert* converter, const uint8_t* yuy2) {
  const int dstStride = converter->inputWidth;
  const size_t chanOffset = converter->inputWidth * converter->inputHeight;
  libyuv::YUY2ToI420(yuy2, converter->inputStride, converter->i420Buffer,
                     dstStride, converter->i420Buffer + chanOffset, dstStride,
                     converter->i420Buffer + 2 * chanOffset, dstStride,
                     converter->inputWidth, converter->inputHeight);

  const int downscaleStride = converter->outputWidth;
  const size_t downscaleChanOffset =
      converter->outputWidth * converter->outputHeight;
  libyuv::I420Scale(
      converter->i420Buffer, dstStride, converter->i420Buffer + chanOffset,
      dstStride, converter->i420Buffer + 2 * chanOffset, dstStride,
      converter->inputWidth, converter->inputHeight,
      converter->i420DownscaleBuffer, downscaleStride,
      converter->i420DownscaleBuffer + downscaleChanOffset, downscaleStride,
      converter->i420DownscaleBuffer + 2 * downscaleChanOffset, downscaleStride,
      converter->outputWidth, converter->outputHeight,
      libyuv::FilterModeEnum(converter->quality));

  libyuv::I420ToARGB(
      converter->i420DownscaleBuffer, downscaleStride,
      converter->i420DownscaleBuffer + downscaleChanOffset, downscaleStride,
      converter->i420DownscaleBuffer + 2 * downscaleChanOffset, downscaleStride,
      converter->rgbaBuffer, converter->outputStride, converter->outputWidth,
      converter->outputHeight);

  return converter->rgbaBuffer;
}

void Yuy2ConvertDestroy(Yuy2Convert* converter) {
  free(converter->rgbaBuffer);
  free(converter->i420Buffer);
  free(converter->i420DownscaleBuffer);
  free(converter);
}

void Yuy2SetQuality(Yuy2Convert* converter, int quality) {
  converter->quality =
      Clamp(quality, int(libyuv::kFilterNone), int(libyuv::kFilterBox));
}
