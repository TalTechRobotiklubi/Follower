#ifndef FL_STREAM_READER
#define FL_STREAM_READER

struct fl_stream_reader;
struct kinect_frame;

fl_stream_reader* fl_stream_reader_open(const char* filename);
kinect_frame* fl_stream_acquire_frame(fl_stream_reader* reader, float dt);
void fl_stream_reader_destroy(fl_stream_reader* reader);

#endif
