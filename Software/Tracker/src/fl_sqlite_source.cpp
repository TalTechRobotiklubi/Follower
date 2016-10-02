#include "fl_sqlite_source.h"
#include "sqlite3/sqlite3.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "fl_math.h"

fl_sqlite_source::fl_sqlite_source(const char* database) {
  int res = sqlite3_open_v2(database, &db, SQLITE_OPEN_READONLY, NULL);
  if (res != SQLITE_OK) {
    printf("failed to open %s: %s\n", database, sqlite3_errmsg(db));
  }

  res = sqlite3_prepare_v2(
      db, "SELECT rowid, data FROM depth_frames WHERE rowid = (?)", -1,
      &frame_query, NULL);

  if (res != SQLITE_OK) {
    printf("failed to compile query: %s\n", sqlite3_errmsg(db));
  }

  if (!res) {
    sqlite3_stmt* count_stmt = NULL;
    sqlite3_prepare_v2(db, "SELECT count(*) FROM depth_frames", -1, &count_stmt,
                       NULL);

    res = sqlite3_step(count_stmt);

    if (res == SQLITE_ROW) {
      total_frames = sqlite3_column_int(count_stmt, 0);
      printf("depth frames: %d\n", total_frames);
      depth_data_len = 512 * 424;
      depth_data = (uint16_t*)calloc(depth_data_len, sizeof(uint16_t));
    }

    sqlite3_finalize(count_stmt);
  }
}

fl_sqlite_source::~fl_sqlite_source() {
  sqlite3_close_v2(db);
  free(depth_data);
}

const kinect_frame* fl_sqlite_source::get_frame() {
  if (!db || total_frames == 0) return NULL;

  if (!manual_advance) advance();

  return &kframe;
}

void fl_sqlite_source::advance() {
  sqlite3_reset(frame_query);
  sqlite3_bind_int(frame_query, 1, current_frame_num);

  int res = sqlite3_step(frame_query);
  if (res == SQLITE_ROW) {
    const void* blob = sqlite3_column_blob(frame_query, 1);
    int bytes = sqlite3_column_bytes(frame_query, 1);
    const int req_depth_bytes = depth_data_len * sizeof(uint16_t);
    assert(bytes == req_depth_bytes);
    int bytes_to_copy = bytes;
    if (bytes_to_copy > req_depth_bytes) bytes_to_copy = req_depth_bytes;

    std::lock_guard<std::mutex> lock(frame_lock);
    memcpy(depth_data, blob, bytes_to_copy);
    kframe.depth_data = depth_data;
    kframe.depth_length = depth_data_len;

    current_frame_num++;
    if (current_frame_num > total_frames) current_frame_num = 1;
  }
}

void fl_sqlite_source::fill_frame(kinect_frame* dst) {
  std::lock_guard<std::mutex> lock(frame_lock);
  copy_frame(&kframe, dst);
}
