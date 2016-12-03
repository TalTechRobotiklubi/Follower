#include "SqliteFrameWriter.h"
#include <stdio.h>
#include <stdlib.h>
#include "KinectFrame.h"
#include "sqlite3/sqlite3.h"

struct SqliteFrameWriter {
  sqlite3* db;
  sqlite3_stmt* insertQuery;
};

SqliteFrameWriter* SqliteFrameWriterCreate(const char* db) {
  SqliteFrameWriter* writer =
      (SqliteFrameWriter*)calloc(1, sizeof(SqliteFrameWriter));
  int res = sqlite3_open(db, &writer->db);

  if (res != SQLITE_OK)
    printf("failed to open DB: %s\n", sqlite3_errmsg(writer->db));

  char* err_msg = nullptr;

  res = sqlite3_exec(writer->db,
                     "CREATE TABLE IF NOT EXISTS depth_frames (data BLOB)",
                     nullptr, nullptr, &err_msg);

  if (err_msg) {
    printf("%s\n", err_msg);
    sqlite3_free(err_msg);
  }

  res = sqlite3_prepare_v2(writer->db, "INSERT INTO depth_frames VALUES (?)",
                           -1, &writer->insertQuery, nullptr);

  if (res != SQLITE_OK) {
    printf("failed to compile query: %s\n", sqlite3_errmsg(writer->db));
  }

  return writer;
}

void SqliteFrameWriterDestroy(SqliteFrameWriter* writer) {
  sqlite3_finalize(writer->insertQuery);
  sqlite3_close_v2(writer->db);
  free(writer);
}

bool SqliteFrameWriterAddFrame(SqliteFrameWriter* writer,
                               const KinectFrame* frame) {
  const int length = int(frame->depthLength * sizeof(uint16_t));
  sqlite3_reset(writer->insertQuery);
  sqlite3_bind_blob(writer->insertQuery, 1, frame->depthData, length,
                    SQLITE_STATIC);

  int res = sqlite3_step(writer->insertQuery);

  return res == SQLITE_DONE;
}
