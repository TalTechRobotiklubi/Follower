#pragma once

struct fl_sqlite_writer;
struct KinectFrame;

fl_sqlite_writer* fl_sqlite_writer_create(const char* db);
void fl_sqlite_writer_destroy(fl_sqlite_writer* writer);
bool fl_sqlite_writer_add_frame(fl_sqlite_writer* writer, const KinectFrame* frame);
