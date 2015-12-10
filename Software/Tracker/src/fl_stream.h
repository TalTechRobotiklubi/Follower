#ifndef FL_STREAM
#define FL_STREAM

enum fl_chan_type {
  FL_CHAN_DEPTH,
  FL_CHAN_RGBA,
  FL_CHAN_INFRARED,
  FL_CHAN_CMD,

  FL_CHAN_MAX
};

struct fl_stream_channel_info {
  int type;
  int num_frames;
  // Length in bytes without time offset bytes.
  // Actual frame length when reading = frame_length + 4
  int frame_length;
  int frame_width;
  int frame_height;
  float duration;
};

struct fl_stream_metadata {
  int metadata_size;
  int num_channels;
  fl_stream_channel_info channels[FL_CHAN_MAX];
};

#endif
