#pragma once

#include "kinect_frame_source.h"
#include "kinect_frame.h"
#include <stdint.h>
#include <stddef.h>
#include <mutex>

struct sqlite3;
struct sqlite3_stmt;

struct fl_sqlite_source : kinect_frame_source {
  fl_sqlite_source(const char* database);
  ~fl_sqlite_source();
  void advance() override;
  const kinect_frame* get_frame() override;
  void fill_frame(kinect_frame* dst) override;

  sqlite3* db = NULL;
  sqlite3_stmt* frame_query = NULL;

  bool manual_advance = false;
  int total_frames = 0;
  int current_frame_num = 1;
  int depth_data_len = 0;
  uint16_t* depth_data = NULL;

  kinect_frame kframe;
  std::mutex frame_lock;
};
