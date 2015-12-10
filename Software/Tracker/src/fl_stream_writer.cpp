#include "fl_stream_writer.h"
#include "fl_stream.h"
#include "kinect_frame.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <string>
#include <array>

const char* fl_chan_names[] = {"depth", "infrared", "rgba", "cmd"};

struct fl_stream_chanwrite {
  fl_stream_chanwrite() { write_buffer.reserve(1 << 20); }

  std::string file_name;
  std::vector<uint8_t> write_buffer;
  std::mutex buffer_lock;
};

struct fl_stream_writer {
  const size_t bytes_reserved = 1 << 25;
  fl_stream_metadata metadata;

  fl_stream_chanwrite channel_writes[FL_CHAN_MAX];

  std::string stream_file;

  std::atomic<bool> running;
  std::thread write_thread;
};

void fl_stream_diskwrite(fl_stream_writer* writer) {
  std::vector<uint8_t> write_buffer;
  write_buffer.reserve(1 << 20);

  std::vector<FILE*> handles;

  for (int i = 0; i < FL_CHAN_MAX; i++) {
    FILE* handle = fopen(writer->channel_writes[i].file_name.c_str(), "w+b");
    handles.push_back(handle);
  }

  while (writer->running) {
    for (int i = 0; i < FL_CHAN_MAX; i++) {
      fl_stream_chanwrite* chanwrite = &writer->channel_writes[i];
      std::unique_lock<std::mutex> buffer_lock(chanwrite->buffer_lock);
      write_buffer.insert(write_buffer.end(), chanwrite->write_buffer.begin(),
                          chanwrite->write_buffer.end());
      chanwrite->write_buffer.clear();
      buffer_lock.unlock();
      const size_t num_bytes = write_buffer.size();
      if (num_bytes > 0) {
        const uint8_t* data = write_buffer.data();
        FILE* handle = handles[i];
        fwrite(data, 1, num_bytes, handle);
        write_buffer.clear();
      }
    }
  }

  printf("num depth frames: %d\n",
         writer->metadata.channels[FL_CHAN_DEPTH].num_frames);

  FILE* stream_file = fopen(writer->stream_file.c_str(), "wb");
  const uint8_t* metadata = (const uint8_t*)&writer->metadata;

  fwrite(metadata, 1, sizeof(writer->metadata), stream_file);

  const size_t temp_buffer_size = 1 << 22;
  write_buffer.resize(temp_buffer_size);

  for (int i = 0; i < FL_CHAN_MAX; i++) {
    fl_stream_chanwrite* chanwrite = &writer->channel_writes[i];
    FILE* handle = handles[i];
    fseek(handle, 0, SEEK_SET);
    size_t bytes_read = 0;
    do {
      bytes_read = fread(write_buffer.data(), 1, temp_buffer_size, handle);
      fwrite(write_buffer.data(), 1, bytes_read, stream_file);
    } while (bytes_read > 0);

    fclose(handle);
    remove(chanwrite->file_name.c_str());
  }

  fclose(stream_file);
}

fl_stream_writer* fl_stream_writer_create(const char* file) {
  fl_stream_writer* writer = new fl_stream_writer();
  writer->metadata.metadata_size = sizeof(fl_stream_metadata);
  writer->metadata.num_channels = FL_CHAN_MAX;

  writer->metadata.channels[FL_CHAN_DEPTH] = {FL_CHAN_DEPTH, 0, 512 * 424 * 2,
                                              512, 424, 0.f};
  writer->metadata.channels[FL_CHAN_RGBA] = {FL_CHAN_RGBA, 0, 320 * 240 * 4,
                                             320, 240, 0.f};
  writer->metadata.channels[FL_CHAN_INFRARED] = {FL_CHAN_INFRARED, 0,
                                                 512 * 424 * 2, 512, 424, 0.f};
  writer->metadata.channels[FL_CHAN_CMD] = {FL_CHAN_CMD, 0, 16, 0, 0, 0.f};

  writer->stream_file = std::string(file);

  for (int i = 0; i < FL_CHAN_MAX; i++) {
    fl_stream_chanwrite* chanwrite = &writer->channel_writes[i];
    chanwrite->file_name = fl_chan_names[i] + writer->stream_file;
  }

  writer->running = true;
  writer->write_thread = std::thread(fl_stream_diskwrite, writer);

  return writer;
}

void fl_stream_writer_destroy(fl_stream_writer* writer) {
  writer->running = false;
  writer->write_thread.join();
  delete writer;
}

void fl_chanwrite_add_raw_frame(fl_stream_chanwrite* chanwrite,
                                const uint8_t* data, size_t length,
                                float time) {
  const uint8_t* end = data + length;

  const uint8_t* time_begin = (const uint8_t*)&time;
  const uint8_t* time_end = time_begin + sizeof(time);

  std::lock_guard<std::mutex> depth_lock(chanwrite->buffer_lock);
  chanwrite->write_buffer.insert(chanwrite->write_buffer.end(), time_begin,
                                 time_end);
  chanwrite->write_buffer.insert(chanwrite->write_buffer.end(), data, end);
}

void fl_stream_add_kframe(fl_stream_writer* writer, const kinect_frame* frame,
                          float time) {
  if (frame->depth_data) {
    writer->metadata.channels[FL_CHAN_DEPTH].num_frames += 1;
    writer->metadata.channels[FL_CHAN_DEPTH].duration = time;
    fl_stream_chanwrite* chanwrite = &writer->channel_writes[FL_CHAN_DEPTH];
    const uint8_t* depth_data = (const uint8_t*)frame->depth_data;
    fl_chanwrite_add_raw_frame(chanwrite, depth_data,
                               frame->depth_length * sizeof(uint16_t), time);
  }

  if (frame->rgba_data) {
    writer->metadata.channels[FL_CHAN_RGBA].num_frames += 1;
    writer->metadata.channels[FL_CHAN_RGBA].duration = time;
    fl_stream_chanwrite* chanwrite = &writer->channel_writes[FL_CHAN_RGBA];
    fl_chanwrite_add_raw_frame(chanwrite, frame->rgba_data, frame->rgba_length,
                               time);
  }

  if (frame->infrared_data) {
    writer->metadata.channels[FL_CHAN_INFRARED].num_frames += 1;
    writer->metadata.channels[FL_CHAN_INFRARED].duration = time;
    fl_stream_chanwrite* chanwrite = &writer->channel_writes[FL_CHAN_INFRARED];
    const uint8_t* infrared_data = (const uint8_t*)frame->infrared_data;
    fl_chanwrite_add_raw_frame(chanwrite, infrared_data,
                               frame->infrared_length * sizeof(uint16_t), time);
  }
}
