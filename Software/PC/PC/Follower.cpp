#include "Follower.h"
#include "WorkerThread.h"


Follower::Follower(void)
{
    workerThread = new WorkerThread();
}


Follower::~Follower(void)
{
}
