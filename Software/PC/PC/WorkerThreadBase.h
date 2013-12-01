#pragma once

#include <QThread>
#include <QMutex>
#include <QMetaType>

#include "SpineCmd.h"

#include "SpineData_Fwd.h"
#include "SpineComm_Fwd.h"

class LogWindow;
class DataLayerCAN;

class WorkerThreadBase : public QThread
{
	Q_OBJECT

public:

signals:
	/*void				spineStateChanged(int state);
	void				speedCmdChanged(double absSpeed, double dir, double turnRate);
	void				spineDataChanged(SpineData* spineData);*/
    void                stopped();

protected:

	virtual void		onNewData(bool newSpineData, SpineData* spineData)	{};
	virtual void		onNewSpineCmd(SpineCmd* spineCmd) {};
    virtual void		onSpineConnected() {};
	virtual void		onSpineDisconnected() {};
	//
	// Override this method, different SpineComm, SpineData, SpineCmd implementations
	// can be used
	//
	virtual void		initData() = 0;

public:

	WorkerThreadBase(QObject *parent = NULL);
	~WorkerThreadBase();

	bool				Start();
	void				Stop();
    void                SetDataLayer(DataLayerCAN* dataLayer) { dataLayer_ = dataLayer; }

	//void				SendSpineCmd(SpineCmd* spineCmd);

	//int					GetState() { return state_; }

	//void				SetLogWindow(LogWindow* logWindow);
    //SpineCmd*           GetRemoteSpineCmd() { return remoteSpineCmd_; }

protected:
	//int					state_;
	bool				threadStopCmd_;
    unsigned int        serialPortNumber_;

	//virtual void		run();

    SpineComm*          spineComm_;
	SpineCmd*			spineCmd_;
	SpineData*			spineData_;
	bool				newSpineData_;

	QMutex				remoteSpineCmdMutex_;
	SpineCmd*			remoteSpineCmd_;
	bool				newRemoteSpineCmd_;

	bool				dataInited_;
    bool                periodOver_;
    int                 timerId_;
    DataLayerCAN*       dataLayer_;
};
