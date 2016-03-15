#pragma once

#include <qobject.h>

class SpineComm;
class DataLayerBase;

class WorkerObjectBase : public QObject
{
    Q_OBJECT
public:
    WorkerObjectBase(SpineComm *spineComm);
    virtual ~WorkerObjectBase(void);

    // called before opening serial port
    virtual void onStart() = 0;

    // called after closing serial port
    virtual void onStop() = 0;
    virtual DataLayerBase *getDataLayer() = 0;

public slots:
    // signals to show if start/stop was successful or not
    void startCommunication(QString nPort);
    void stopCommunication(void);

signals:
    void startStatus(bool connStatus);
    void stopStatus();
    void newDataReceived(void);

protected:
    void timerEvent(QTimerEvent *event);

private:
    SpineComm*      spineComm_;    
    int             timerId_;
    bool            running_;
};

