#include "fl_sqlite_writer.h"
#include <stdio.h>
#include <stdlib.h>
#include "KinectFrame.h"
#include "sqlite3/sqlite3.h"

struct fl_sqlite_writer {
  sqlite3* db;
  sqlite3_stmt* insert_query;
};

fl_sqlite_writer* fl_sqlite_writer_create(const char* db) {
  fl_sqlite_writer* writer =
      (fl_sqlite_writer*)calloc(1, sizeof(fl_sqlite_writer));
  int res = sqlite3_open(db, &writer->db);

  if (res != SQLITE_OK)
    printf("failed to open DB: %s\n", sqlite3_errmsg(writer->db));

  char* err_msg = NULL;

  res = sqlite3_exec(writer->db,
                     "CREATE TABLE IF NOT EXISTS depth_frames (data BLOB)",
                     NULL, NULL, &err_msg);

  if (err_msg) {
    printf("%s\n", err_msg);
    sqlite3_free(err_msg);
  }

  res = sqlite3_prepare_v2(writer->db, "INSERT INTO depth_frames VALUES (?)",
                           -1, &writer->insert_query, NULL);

  if (res != SQLITE_OK) {
    printf("failed to compile query: %s\n", sqlite3_errmsg(writer->db));
  }

  return writer;
}

void fl_sqlite_writer_destroy(fl_sqlite_writer* writer) {
  sqlite3_finalize(writer->insert_query);
  sqlite3_close_v2(writer->db);
  free(writer);
}

bool fl_sqlite_writer_add_frame(fl_sqlite_writer* writer,
                                const KinectFrame* frame) {
  const int length = int(frame->depthLength * sizeof(uint16_t));
  sqlite3_reset(writer->insert_query);
  sqlite3_bind_blob(writer->insert_query, 1, frame->depthData, length,
                    SQLITE_STATIC);

  int res = sqlite3_step(writer->insert_query);

  return res == SQLITE_DONE;
}
