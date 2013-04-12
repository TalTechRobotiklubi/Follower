#pragma once
#include "qobject.h"
#include "CANWorkerThread.h"


class Follower :
    public QObject
{
public:
    Follower(void);
    ~Follower(void);

    CANWorkerThread *workerThread;
};

