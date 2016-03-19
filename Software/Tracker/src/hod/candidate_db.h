#ifndef CANDIDATE_DB_H
#define CANDIDATE_DB_H

struct sqlite3;
struct sqlite3_stmt;
struct hod_candidate;
struct hod_result;

struct candidate_db {
  sqlite3* db;
  sqlite3_stmt* existing_query;
  sqlite3_stmt* insert_query;
  sqlite3_stmt* update_query;
  sqlite3_stmt* features_query;
};

void candidate_db_init(candidate_db* db, const char* db_name);
void candidate_db_close(candidate_db* db);
void candidate_db_add_candidate(candidate_db* db, const hod_candidate* candidate, bool is_human);
int candidate_db_get_count(candidate_db* db);
// retrieve all feature
int candidate_db_get_features(candidate_db* db, hod_result* results, int max_results);

#endif
