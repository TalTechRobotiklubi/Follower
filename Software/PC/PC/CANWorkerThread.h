#pragma once
#include "workerthreadbase.h"
#include "SpineDataCAN.h"

class CANWorkerThread :
    public WorkerThreadBase
{
    Q_OBJECT
signals:
    void update_UIdata(SpineDataCAN* spineData);

public:

    CANWorkerThread(void);
    ~CANWorkerThread(void);
    void            initData(void);
    SpineDataCAN*   GetRemoteSpineData(void) { return remoteSpineData_; }
    virtual void	onNewSpineCmd(SpineCmd* spineCmd);
    virtual void	onNewData(bool newSpineData, SpineData* spineData);
    virtual void    onAlgorithmStart();
    virtual void    onAlgorithmStop();
private:
    SpineDataCAN*   remoteSpineData_;
};

