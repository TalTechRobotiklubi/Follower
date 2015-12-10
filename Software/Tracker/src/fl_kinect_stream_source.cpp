#include "fl_kinect_stream_source.h"
#include "fl_stream_reader.h"
#include "kinect_frame.h"

fl_kinect_stream_source::fl_kinect_stream_source(const char* file)
    : stream_reader(fl_stream_reader_open(file)) {}

fl_kinect_stream_source::~fl_kinect_stream_source() {
  fl_stream_reader_destroy(stream_reader);
}

const kinect_frame* fl_kinect_stream_source::get_frame(float dt) {
  return fl_stream_acquire_frame(stream_reader, dt);
}
