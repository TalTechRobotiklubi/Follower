#pragma once

#include "SpineCmd_Fwd.h"
#include "SpineData_Fwd.h"

#include "Serial.h"

class SpineComm : public CSerial
{
public:
	virtual void	Communicate() = 0;

	virtual bool	HasNewData() const = 0;
	virtual void	ClearNewData() = 0;

	virtual void	GetData(SpineData* out_data) const = 0;

	virtual bool	SendControllerCommands( SpineCmd* spineCmd ) = 0;

    void            DummyFunction(void)  {};
};