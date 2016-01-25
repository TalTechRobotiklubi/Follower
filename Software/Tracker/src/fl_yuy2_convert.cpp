#include "fl_yuy2_convert.h"
#include <stdlib.h>
#include <libyuv.h>
#include "fl_math.h"

struct fl_yuy2_convert {
  int output_width;
  int output_height;
  int output_stride;
  int input_width;
  int input_height;
  int input_stride;
  int quality;

  size_t rgba_buffer_length;
  uint8_t* rgba_buffer;

  size_t i420_buffer_length;
  uint8_t* i420_buffer;

  size_t i420_downscale_buffer_length;
  uint8_t* i420_downscale_buffer;
};

fl_yuy2_convert* fl_yuy2_convert_create(int input_width, int input_height,
                                        int output_width, int output_height) {
  fl_yuy2_convert* converter =
      (fl_yuy2_convert*)malloc(sizeof(fl_yuy2_convert));
  converter->output_width = output_width;
  converter->output_height = output_height;
  converter->output_stride = output_width * 4;
  converter->input_width = input_width;
  converter->input_height = input_height;
  converter->input_stride = input_width * 2;
  converter->quality = 0;

  converter->rgba_buffer_length = output_width * output_height * 4;
  converter->rgba_buffer = (uint8_t*)malloc(converter->rgba_buffer_length);

  converter->i420_buffer_length = input_width * input_height * 3;
  converter->i420_buffer = (uint8_t*)malloc(converter->i420_buffer_length);

  converter->i420_downscale_buffer_length = output_width * output_height * 3;
  converter->i420_downscale_buffer =
      (uint8_t*)malloc(converter->i420_downscale_buffer_length);

  return converter;
}

uint8_t* fl_yuy2_to_rgba(fl_yuy2_convert* converter, const uint8_t* yuy2) {
  const int dst_stride = converter->input_width;
  const size_t chan_offset = converter->input_width * converter->input_height;
  libyuv::YUY2ToI420(yuy2, converter->input_stride, converter->i420_buffer,
                     dst_stride, converter->i420_buffer + chan_offset,
                     dst_stride, converter->i420_buffer + 2 * chan_offset,
                     dst_stride, converter->input_width,
                     converter->input_height);

  const int downscale_stride = converter->output_width;
  const size_t downscale_chan_offset =
      converter->output_width * converter->output_height;
  libyuv::I420Scale(
      converter->i420_buffer, dst_stride, converter->i420_buffer + chan_offset,
      dst_stride, converter->i420_buffer + 2 * chan_offset, dst_stride,
      converter->input_width, converter->input_height,
      converter->i420_downscale_buffer, downscale_stride,
      converter->i420_downscale_buffer + downscale_chan_offset,
      downscale_stride,
      converter->i420_downscale_buffer + 2 * downscale_chan_offset,
      downscale_stride, converter->output_width, converter->output_height,
      libyuv::FilterModeEnum(converter->quality));

  libyuv::I420ToARGB(
      converter->i420_downscale_buffer, downscale_stride,
      converter->i420_downscale_buffer + downscale_chan_offset,
      downscale_stride,
      converter->i420_downscale_buffer + 2 * downscale_chan_offset,
      downscale_stride, converter->rgba_buffer, converter->output_stride,
      converter->output_width, converter->output_height);

  return converter->rgba_buffer;
}

void fl_yuy2_convert_destroy(fl_yuy2_convert* converter) {
  free(converter->rgba_buffer);
  free(converter->i420_buffer);
  free(converter->i420_downscale_buffer);
  free(converter);
}

void fl_yuy2_set_quality(fl_yuy2_convert* converter, int quality) {
  converter->quality =
      fl_clamp(quality, int(libyuv::kFilterNone), int(libyuv::kFilterBox));
}
