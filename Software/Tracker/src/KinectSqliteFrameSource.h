#pragma once

#include <stddef.h>
#include <stdint.h>
#include <mutex>
#include <atomic>
#include "KinectFrame.h"
#include "KinectFrameSource.h"

struct sqlite3;
struct sqlite3_stmt;

struct KinectSqliteFrameSource : KinectFrameSource {
  KinectSqliteFrameSource(const char* database);
  ~KinectSqliteFrameSource();
  const KinectFrame* GetFrame() override;
  void FillFrame(KinectFrame* dst) override;
  int FrameNumber() const override;

  sqlite3* db = nullptr;
  sqlite3_stmt* frameQuery = nullptr;

  std::atomic<int> currentFrameNum;
  int totalFrames = 0;
  int depthDataLen = 0;
  uint16_t* depthData = nullptr;

  KinectFrame kframe;
  std::mutex frameLock;
};
