#pragma once

#include <QObject>
#include <QElapsedTimer>
#include "CSpineComm.h"
//#include "SpineDataCAN.h"
#include "CANmessages.h"
#include "PacketHandler.h"
#include "DataLayerCAN.h"
//#include "SpineCmd_Fwd.h"


class SpineCommCAN : public SpineComm
{
public:
	SpineCommCAN();
	~SpineCommCAN(void);

	virtual void		    Communicate();
	
	virtual bool		    HasNewData() const	{ return hasNewData_; }
	virtual void		    ClearNewData()		{ hasNewData_ = false; }
	virtual DataLayerBase*  getDataLayer();
    virtual bool		    SendControllerCommands();

	//void                    SetLogicDataToDataLayer( SpineCmd* spineCmd);
    
    
private:
    //static const int    PERIOD_IN_MS; 
    void                clearRxStorageElement(UART_CANmessage *message);
    void                storeDataToDataLayer(UART_CANmessage *message, PacketWithIndex *packet);
    bool                sendDataLayerDataToUART(PacketWithIndex *packet);
    uint8_t             getBitmaskForUARTmessage(uint8_t bitPosition, int16_t length);

	bool				hasNewData_;
    //SpineDataCAN	    spineDataCAN_;		

	unsigned int   		readBufSize_;
	unsigned char*		readBuf_;
	unsigned int		bytesInBuf_;
    PacketHandler       packethandler_;
    DataLayerCAN        dataLayerCAN_;
};

