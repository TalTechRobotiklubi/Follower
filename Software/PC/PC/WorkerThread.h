#pragma once
#include <qthread.h>
#include "SpineCmd.h"
#include "SpineData_Fwd.h"
#include "SpineComm_Fwd.h"
#include "SpineDataCAN.h"


class WorkerThread :
    public QThread
{
public:
	WorkerThread(QObject *parent = NULL);
	~WorkerThread();

	bool				Start();
	void				Stop();


protected:
	virtual void onNewData(bool newSpineData, SpineData* spineData);
    bool threadStopCmd_;

    SpineComm*          spineComm_;
	SpineCmd*			spineCmd_;
	SpineData*			spineData_;
	bool				newSpineData_;

    SpineCmd*			remoteSpineCmd_;
    SpineDataCAN*       remoteSpineData_;
    virtual void		run();
};

