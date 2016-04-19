#include "candidate_db.h"
#include "sqlite3/sqlite3.h"
#include "hod_candidate.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fnv1a.h"

bool db_check_existing(candidate_db* db, int64_t hash) {
  sqlite3_reset(db->existing_query);
  sqlite3_bind_int64(db->existing_query, 1, hash);

  int res = sqlite3_step(db->existing_query);

  if (res == SQLITE_ROW) {
    int count = sqlite3_column_int(db->existing_query, 0);
    return count > 0;
  }

  return false;
}

bool db_insert_candidate(candidate_db* db, const hod_candidate* candidate,
                         int64_t hash, const image* depth, bool is_human) {
  sqlite3_reset(db->insert_query);
  sqlite3_bind_int64(db->insert_query, 1, hash);
  sqlite3_bind_int(db->insert_query, 2, candidate->candidate_width);
  sqlite3_bind_int(db->insert_query, 3, candidate->candidate_height);
  sqlite3_bind_blob(db->insert_query, 4, depth->data, depth->bytes,
                    SQLITE_STATIC);
  sqlite3_bind_blob(db->insert_query, 5, candidate->features,
                    candidate->num_features * sizeof(float), SQLITE_STATIC);
  sqlite3_bind_int(db->insert_query, 6, int(is_human));

  int res = sqlite3_step(db->insert_query);

  return res == SQLITE_DONE;
}

bool db_update_candidate(candidate_db* db, const hod_candidate* candidate,
                         int64_t hash, const image* depth, bool is_human) {
  sqlite3_reset(db->update_query);
  sqlite3_bind_int(db->update_query, 1, candidate->candidate_width);
  sqlite3_bind_int(db->update_query, 2, candidate->candidate_height);
  sqlite3_bind_blob(db->update_query, 3, depth->data, depth->bytes,
                    SQLITE_STATIC);
  sqlite3_bind_blob(db->update_query, 4, candidate->features,
                    candidate->num_features * sizeof(float), SQLITE_STATIC);
  sqlite3_bind_int(db->update_query, 5, int(is_human));
  sqlite3_bind_int64(db->update_query, 6, hash);

  int res = sqlite3_step(db->update_query);

  return res == SQLITE_DONE;
}

// candidates;
// hash:integer,
// width:integer,
// height:integer,
// depth:blob,
// features:blob,
// human:integer

void candidate_db_init(candidate_db* db, const char* db_name) {
  int res = sqlite3_open(db_name, &db->db);
  if (res != SQLITE_OK) {
    printf("failed to open %s: %s\n", db_name, sqlite3_errmsg(db->db));
    return;
  }

  char* err_msg = NULL;
  res = sqlite3_exec(
      db->db,
      "CREATE TABLE IF NOT EXISTS candidates (hash integer primary key not "
      "null, width integer not null, height integer not null, "
      "depth blob not null, features blob not null, human integer)",
      NULL, NULL, &err_msg);

  if (err_msg) {
    printf("%s\n", err_msg);
    sqlite3_free(err_msg);
  }

  res = sqlite3_prepare_v2(db->db,
                           "SELECT count(*) FROM candidates WHERE hash = (?)",
                           -1, &db->existing_query, NULL);

  auto sqlite_check = [=](int code) {
    if (code != SQLITE_OK) {
      printf("failed to compile query: %s\n", sqlite3_errmsg(db->db));
    }
  };

  sqlite_check(res);

  res = sqlite3_prepare_v2(db->db,
                           "INSERT INTO candidates VALUES (?, ?, ?, ?, ?, ?)",
                           -1, &db->insert_query, NULL);

  sqlite_check(res);

  res = sqlite3_prepare_v2(db->db,
                           "UPDATE candidates SET width = ?, height = ?, depth "
                           "= ?, features = ?, human = ? WHERE hash = ?",
                           -1, &db->update_query, NULL);

  sqlite_check(res);

  res = sqlite3_prepare_v2(db->db, "SELECT features, human FROM candidates", -1,
                           &db->features_query, NULL);

  sqlite_check(res);
}

void candidate_db_close(candidate_db* db) {
  if (!db) return;

  sqlite3_finalize(db->existing_query);
  sqlite3_finalize(db->insert_query);
  sqlite3_close_v2(db->db);
}

void candidate_db_add_candidate(candidate_db* db,
                                const hod_candidate* candidate, bool is_human) {
  int width = candidate->candidate_width;
  int height = candidate->candidate_height;
  image candidate_img;
  image_init(&candidate_img, width, height);

  image_region src_reg;
  src_reg.x = 1;
  src_reg.y = 1;
  src_reg.width = candidate->depth.width - 1;
  src_reg.height = candidate->depth.height - 1;

  image_region dst_reg;
  dst_reg.x = 0;
  dst_reg.y = 0;
  dst_reg.width = width;
  dst_reg.height = height;

  copy_sub_image(&candidate->depth, &src_reg, &candidate_img, &dst_reg);

  int64_t hash = int64_t(fnv1a_hash(candidate_img.data, candidate_img.bytes));

  bool already_exists = db_check_existing(db, hash);

  if (already_exists) {
    db_update_candidate(db, candidate, hash, &candidate_img, is_human);
  } else {
    db_insert_candidate(db, candidate, hash, &candidate_img, is_human);
  }

  image_destroy(&candidate_img);
}

int candidate_db_get_count(candidate_db* db) {
  const auto cb = [](void* usr, int col_count, char** data, char**) {
    if (col_count > 0) {
      int count = strtol(data[0], NULL, 10);
      *(int*)usr = count;
    }
    return 0;
  };

  int count = 0;
  char* err_msg = NULL;
  sqlite3_exec(db->db, "SELECT count(*) from candidates", cb, &count, &err_msg);

  if (err_msg) {
    printf("%s\n", err_msg);
    sqlite3_free(err_msg);
  }

  return count;
}

int candidate_db_get_features(candidate_db* db, hod_result* results,
                              int max_results) {
  sqlite3_reset(db->features_query);

  int idx = 0;
  while (sqlite3_step(db->features_query) == SQLITE_ROW && idx < max_results) {
    const void* features = sqlite3_column_blob(db->features_query, 0);
    int num_bytes = sqlite3_column_bytes(db->features_query, 0);
    int is_human = sqlite3_column_int(db->features_query, 1);
    memcpy(results[idx].features, features, num_bytes);
    results[idx].num_features = num_bytes / sizeof(float);
    results[idx].human = is_human;
    idx++;
  }

  return idx;
}