#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char** argv) {
  const char* input = argv[1];

  if (!input) {
    return 1;
  }

  char output[256];
  snprintf(output, 256, "out_%s", input);

  sqlite3* db = NULL;
  sqlite3_open_v2(input, &db, SQLITE_OPEN_READONLY, NULL);
  if (!db) {
    printf("Can't open %s\n", input);
    return 1;
  }

  sqlite3* res_db = NULL;
  sqlite3_open(output, &res_db);

  sqlite3_exec(res_db,
    "create table if not exists labelled(depth blob, human int)", NULL, NULL, NULL);

  sqlite3_stmt* stmt = NULL;
  sqlite3_prepare_v2(db, "select depth, human from candidates", -1, &stmt, NULL);

  sqlite3_stmt* insert = NULL;
  sqlite3_prepare_v2(res_db, "insert into labelled VALUES(?, ?)", -1, &insert, NULL);

  size_t len = 64 * 128;
  float* buffer = (float*)calloc(64 * 128, sizeof(float));
  float m = 0.f;

  sqlite3_exec(res_db, "begin", 0, 0, 0);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    const void* depth = sqlite3_column_blob(stmt, 0);
    int nb = sqlite3_column_bytes(stmt, 0);
    int human = sqlite3_column_int(stmt, 1);

    if (nb != len * 2) {
      printf("dimension mismatch\n");
      abort();
    }

    const uint16_t* raw = (const uint16_t*)depth;

    for (size_t i = 0; i < len; i++) {
      uint16_t v = raw[i];
      float n = (float)v;
      buffer[i] = n;
    }

    sqlite3_reset(insert);
    sqlite3_bind_blob(insert, 1, buffer, len * sizeof(float), SQLITE_STATIC);
    sqlite3_bind_int(insert, 2, human);
    sqlite3_step(insert);
  }
  sqlite3_exec(res_db, "commit", 0, 0, 0);

  sqlite3_finalize(stmt);
  sqlite3_close_v2(db);
  sqlite3_close(res_db);

  free(buffer);

  return 0;
}
