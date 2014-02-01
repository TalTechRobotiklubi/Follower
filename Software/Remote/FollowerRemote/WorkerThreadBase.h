#pragma once

#include <QThread>


class WorkerThreadBase : public QThread
{
	Q_OBJECT

public:

	WorkerThreadBase(QObject *parent = NULL);
	~WorkerThreadBase();

	void				StartThread();
	void				StopThread();

protected:
    virtual void        onStart() = 0;
    virtual void        onStop() = 0;

private:

};
