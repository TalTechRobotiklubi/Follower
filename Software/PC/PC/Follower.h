#pragma once
#include "qobject.h"
#include "WorkerThread.h"


class Follower :
    public QObject
{
public:
    Follower(void);
    ~Follower(void);

    WorkerThread *workerThread;
};

