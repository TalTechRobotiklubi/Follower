#include "KinectSqliteFrameSource.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <thread>
#include "fl_math.h"
#include "sqlite3/sqlite3.h"

KinectSqliteFrameSource::KinectSqliteFrameSource(const char* database)
    : currentFrameNum(1) {
  int res = sqlite3_open_v2(database, &db, SQLITE_OPEN_READONLY, nullptr);
  if (res != SQLITE_OK) {
    printf("failed to open %s: %s\n", database, sqlite3_errmsg(db));
  }

  res = sqlite3_prepare_v2(
      db, "SELECT rowid, data FROM depth_frames WHERE rowid = (?)", -1,
      &frameQuery, nullptr);

  if (res != SQLITE_OK) {
    printf("failed to compile query: %s\n", sqlite3_errmsg(db));
  }

  if (!res) {
    sqlite3_stmt* countStmt = NULL;
    sqlite3_prepare_v2(db, "SELECT count(*) FROM depth_frames", -1, &countStmt,
                       nullptr);

    res = sqlite3_step(countStmt);

    if (res == SQLITE_ROW) {
      totalFrames = sqlite3_column_int(countStmt, 0);
      printf("depth frames: %d\n", totalFrames);
      depthDataLen = 512 * 424;
      depthData = (uint16_t*)calloc(depthDataLen, sizeof(uint16_t));
    }

    sqlite3_finalize(countStmt);
  }
}

KinectSqliteFrameSource::~KinectSqliteFrameSource() {
  sqlite3_close_v2(db);
  free(depthData);
}

const KinectFrame* KinectSqliteFrameSource::GetFrame() {
  if (!db || totalFrames == 0) return nullptr;

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  sqlite3_reset(frameQuery);
  sqlite3_bind_int(frameQuery, 1, currentFrameNum.load());

  int res = sqlite3_step(frameQuery);
  if (res == SQLITE_ROW) {
    const void* blob = sqlite3_column_blob(frameQuery, 1);
    int bytes = sqlite3_column_bytes(frameQuery, 1);
    const int reqDepthBytes = depthDataLen * sizeof(uint16_t);
    assert(bytes == reqDepthBytes);
    int bytesToCopy = bytes;
    if (bytesToCopy > reqDepthBytes) bytesToCopy = reqDepthBytes;

    std::lock_guard<std::mutex> lock(frameLock);
    memcpy(depthData, blob, bytesToCopy);
    kframe.depthData = depthData;
    kframe.depthLength = depthDataLen;

    currentFrameNum++;
    if (currentFrameNum > totalFrames) currentFrameNum = 1;
  }

  return &kframe;
}

void KinectSqliteFrameSource::FillFrame(KinectFrame* dst) {
  std::lock_guard<std::mutex> lock(frameLock);
  CopyKinectFrame(&kframe, dst);
}

int KinectSqliteFrameSource::FrameNumber() const {
  return currentFrameNum.load();
}
