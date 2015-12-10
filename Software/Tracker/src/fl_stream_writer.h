#ifndef FL_STREAM_WRITER
#define FL_STREAM_WRITER

struct kinect_frame;
struct fl_stream_writer;
struct kinect_frame;

fl_stream_writer* fl_stream_writer_create(const char* file);
void fl_stream_writer_destroy(fl_stream_writer* writer);
void fl_stream_add_kframe(fl_stream_writer* writer, const kinect_frame* frame,
                          float time);

#endif
