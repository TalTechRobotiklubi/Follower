#include <iostream>

#include <UnitTest++.h>
#include "projectConfig.h"

extern "C" {
#include "InterfaceHandler.h"
#include "DataLayerConfig.h"
}

TEST(dataIsStoredInBigEndianFormat_int16)
{
	SplitInt16 qw;
	qw.i = 1110;

	InterfaceMessage msg;
	msg.packet = PacketQuaternions;
	msg.period = PacketDescriptorList[PacketQuaternions].period;
	msg.id = PacketDescriptorList[PacketQuaternions].id;
	msg.length = PacketDescriptorList[PacketQuaternions].dlc;
	msg.data[0] = qw.d[1];
	msg.data[1] = qw.d[0];
	InterfaceHandler_storeReceivedData(&msg);

	DLParamDescriptor qwParam = psDLParamDescriptorList[DLParamQw];
	CHECK_EQUAL(qw.i, *((int16_t*)qwParam.pValue));
}

