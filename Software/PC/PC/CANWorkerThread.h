#pragma once
#include "workerthreadbase.h"

class CANWorkerThread :
    public WorkerThreadBase
{
    Q_OBJECT

public:

    CANWorkerThread(void);
    ~CANWorkerThread(void);

protected:
    virtual void	onStart();
    virtual void    onStop();

};

