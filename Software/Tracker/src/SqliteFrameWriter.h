#pragma once

struct SqliteFrameWriter;
struct KinectFrame;

SqliteFrameWriter* SqliteFrameWriterCreate(const char* db);
void SqliteFrameWriterDestroy(SqliteFrameWriter* writer);
bool SqliteFrameWriterAddFrame(SqliteFrameWriter* writer,
                               const KinectFrame* frame);
