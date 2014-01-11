#pragma once

//#include "SpineCmd_Fwd.h"
//#include "SpineData_Fwd.h"

#include "Serial.h"

class DataLayerBase;

class SpineComm : public CSerial
{
public:
    SpineComm() {};
    virtual ~SpineComm() {}

    virtual int             GetPeriodInMs() { return 10; }
	virtual void	        Communicate() = 0;

	virtual bool	        HasNewData() const = 0;
	virtual void	        ClearNewData() = 0;
	virtual bool	        SendControllerCommands() = 0;
    virtual DataLayerBase*  getDataLayer() = 0;
    void                    DummyFunction(void)  {}
};