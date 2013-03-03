#include "Follower.h"
#include "CANWorkerThread.h"


Follower::Follower(void)
{
    workerThread = new CANWorkerThread();
}


Follower::~Follower(void)
{
}
