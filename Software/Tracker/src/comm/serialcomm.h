#pragma once

#include "interfacehandler.h"

class SerialComm
{
public:
	SerialComm();
	~SerialComm(void);

	void parseMessages(const char*, size_t size);
    void sendControllerCommands();
    
private:	
	void clearMessageStorage(InterfaceMessage& message);
};

