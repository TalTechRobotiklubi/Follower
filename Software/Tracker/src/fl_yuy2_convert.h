#ifndef FL_YUY2_CONVERT
#define FL_YUY2_CONVERT

#include <stdint.h>
#include <stddef.h>

struct fl_yuy2_convert;

fl_yuy2_convert* fl_yuy2_convert_create(int input_width, int input_height,
                                        int output_width, int output_height);

uint8_t* fl_yuy2_to_rgba(fl_yuy2_convert* converter, const uint8_t* yuy2);
void fl_yuy2_convert_destroy(fl_yuy2_convert* converter);
void fl_yuy2_set_quality(int quality);

#endif
