#pragma once
#include "qobject.h"
#include <QSettings>

class WorkerThreadBase;
class WorkerObjectBase;
class DataLayerBase;
class Kinematics;

class Follower :
    public QObject
{
public:
  Follower(void);
  ~Follower(void);

  WorkerThreadBase *getWorkerThread()  { return workerThread_; }
  WorkerObjectBase *getWorkerObject()  { return workerObject_; }
  Kinematics *getKinematics()    { return kinematics_; }
  QSettings *getSettings()  { return settings_; }
private:
  WorkerThreadBase *workerThread_;
  WorkerObjectBase *workerObject_;
  Kinematics *kinematics_;
  QSettings *settings_;
};

