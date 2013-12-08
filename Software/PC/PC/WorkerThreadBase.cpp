#include "WorkerThreadBase.h"


WorkerThreadBase::WorkerThreadBase(QObject *parent)	: QThread(parent)
{

}

void WorkerThreadBase::StartThread()
{
    onStart();
	QThread::start();
}



void WorkerThreadBase::StopThread()
{
    onStop();
    exit();
    wait();
}


WorkerThreadBase::~WorkerThreadBase()
{
}