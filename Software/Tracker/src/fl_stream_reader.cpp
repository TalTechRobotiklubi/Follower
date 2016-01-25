#include "fl_stream_reader.h"
#include "fl_stream.h"
#include "kinect_frame.h"
#include <stdio.h>
#include <string.h>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <queue>
#include "fl_block_alloc.h"

struct fl_channel_read {
  FILE* stream = nullptr;
  float duration = 0.f;
  int total_frames = 0;
  int current_frame = 0;

  size_t content_length = 0;
  size_t frame_length = 0;
  size_t initial_file_offset = 0;
  size_t file_offset = 0;
  float current_time = 0.f;
  float next_frame_time = 0.f;

  const size_t num_blocks = 32;

  std::queue<uint8_t*> fetch_queue;
  std::vector<uint8_t> single_frame_data;
  fl_block_alloc* block_alloc = nullptr;

  fl_chan_type type;
};

struct fl_stream_reader {
  FILE* stream_file = nullptr;
  fl_stream_metadata metadata;

  kinect_frame frame;
  fl_channel_read channel_reads[FL_CHAN_MAX];

  std::atomic<bool> running;
  std::thread reader_thread;

  std::condition_variable read_cv;
  std::mutex reader_lock;
};

void fl_stream_prefetch(fl_stream_reader* reader) {
  for (size_t i = 0; i < FL_CHAN_MAX; i++) {
    fl_channel_read* chan = &reader->channel_reads[i];
    chan->file_offset = chan->initial_file_offset;
  }

  for (;;) {
    std::unique_lock<std::mutex> cv_lock(reader->reader_lock);
    reader->read_cv.wait(cv_lock);
    cv_lock.unlock();
    if (!reader->running) break;

    for (int i = 0; i < FL_CHAN_MAX; i++) {
      fl_channel_read* chan = &reader->channel_reads[i];
      if (chan->total_frames == 0) continue;

      cv_lock.lock();
      const size_t alive_blocks = chan->fetch_queue.size();
      cv_lock.unlock();

      const size_t num_blocks_to_fetch = chan->num_blocks - alive_blocks;
      const size_t frame_length = chan->frame_length;
      const size_t channel_length =
          size_t(chan->total_frames) * chan->frame_length;

      for (size_t block_idx = 0; block_idx < num_blocks_to_fetch; block_idx++) {
        cv_lock.lock();
        uint8_t* block_data =
            (uint8_t*)fl_block_alloc_acquire(chan->block_alloc);
        cv_lock.unlock();
        if (block_data == nullptr)
          fprintf(stderr, "Failed to allocate block for channel %d\n", i);

        fseek(chan->stream, long(chan->file_offset), SEEK_SET);
        size_t actual_read = fread(block_data, 1, frame_length, chan->stream);

        chan->file_offset += actual_read;

        cv_lock.lock();
        chan->fetch_queue.push(block_data);
        cv_lock.unlock();

        if (chan->file_offset - chan->initial_file_offset >= channel_length) {
          chan->file_offset = chan->initial_file_offset;
          fseek(chan->stream, long(chan->file_offset), SEEK_SET);
        }
      }
    }
  }
}

fl_stream_reader* fl_stream_reader_open(const char* filename) {
  FILE* stream_file = fopen(filename, "rb");

  if (!stream_file) return nullptr;

  fl_stream_reader* reader = new fl_stream_reader();
  reader->stream_file = stream_file;

  fread(&reader->metadata, 1, sizeof(fl_stream_metadata), stream_file);

  size_t current_offset = sizeof(fl_stream_metadata);

  for (int i = 0; i < FL_CHAN_MAX; i++) {
    const fl_stream_channel_info* chan_info = &reader->metadata.channels[i];
    printf("duration %f\n", chan_info->duration);

    const size_t content_length = size_t(chan_info->frame_length);
    const size_t frame_length = content_length + 4;

    fl_channel_read* chan = &reader->channel_reads[i];
    chan->stream = stream_file;
    chan->duration = chan_info->duration;
    chan->total_frames = chan_info->num_frames;
    chan->content_length = content_length;
    chan->frame_length = frame_length;
    chan->single_frame_data.resize(content_length);
    chan->block_alloc = fl_block_alloc_create(frame_length, chan->num_blocks);
    chan->type = (fl_chan_type)i;

    size_t num_frames = size_t(chan_info->num_frames);
    printf("num_frames: %lu\n", num_frames);
    chan->initial_file_offset = current_offset;
    current_offset += num_frames * frame_length;
  }

  reader->running = true;
  reader->reader_thread = std::thread(fl_stream_prefetch, reader);

  return reader;
}

size_t fl_channel_advance_frames(fl_stream_reader* reader,
                                 fl_channel_read* chan) {
  size_t frames_advanced = 0;
  while (chan->next_frame_time < chan->current_time) {
    std::unique_lock<std::mutex> rd_lock(reader->reader_lock);

    if (chan->fetch_queue.empty()) {
      reader->read_cv.notify_one();
      return frames_advanced;
    }

    uint8_t* frame_data = chan->fetch_queue.front();
    chan->current_frame = (chan->current_frame + 1) % chan->total_frames;

    float frame_time;
    memcpy(&frame_time, frame_data, 4);
    chan->next_frame_time = frame_time;

    std::copy(frame_data + 4, frame_data + chan->content_length,
              chan->single_frame_data.begin());

    fl_block_alloc_release(chan->block_alloc, frame_data);
    chan->fetch_queue.pop();

    if (frame_time == chan->duration) {
      chan->current_time = 0.f;
      chan->next_frame_time = 0.f;
    }

    if (chan->fetch_queue.size() < chan->num_blocks / 2) {
      reader->read_cv.notify_one();
    }

    frames_advanced += 1;
  }

  return frames_advanced;
}

kinect_frame* fl_stream_acquire_frame(fl_stream_reader* reader, float dt) {
  for (int i = 0; i < FL_CHAN_MAX; i++) {
    fl_channel_read* chan = &reader->channel_reads[i];
    if (chan->total_frames == 0) continue;

    chan->current_time += dt;

    const fl_chan_type chan_type = (fl_chan_type)i;

    fl_channel_advance_frames(reader, chan);

    uint8_t* begin = chan->single_frame_data.data();
    uint8_t* end = begin + chan->content_length;
    switch (chan_type) {
      case FL_CHAN_DEPTH: {
        reader->frame.depth_data = (uint16_t*)begin;
        reader->frame.depth_length = (end - begin) / sizeof(uint16_t);
      } break;
      case FL_CHAN_RGBA: {
        reader->frame.rgba_data = begin;
        reader->frame.rgba_length = chan->content_length;
        reader->frame.rgba_width = 320;
        reader->frame.rgba_height = 240;
      } break;
      case FL_CHAN_INFRARED: {
        reader->frame.infrared_data = (uint16_t*)begin;
        reader->frame.infrared_length = chan->content_length / sizeof(uint16_t);
      } break;
      default:
        break;
    }
  }

  return &reader->frame;
}

void fl_stream_reader_destroy(fl_stream_reader* reader) {
  reader->running = false;
  reader->read_cv.notify_one();
  reader->reader_thread.join();
  fclose(reader->stream_file);

  for (fl_channel_read& chan : reader->channel_reads) {
    fl_block_alloc_destroy(chan.block_alloc);
  }

  delete reader;
}
