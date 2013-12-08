#pragma once

#include <QThread>

class DataLayerCAN;

class WorkerThreadBase : public QThread
{
	Q_OBJECT

public:

	WorkerThreadBase(QObject *parent = NULL);
	~WorkerThreadBase();

	void				StartThread();
	void				StopThread();
    void                SetDataLayer(DataLayerCAN* dataLayer) { dataLayer_ = dataLayer; }

protected:
    virtual void        onStart() = 0;
    virtual void        onStop() = 0;

private:

    DataLayerCAN*       dataLayer_;
};
