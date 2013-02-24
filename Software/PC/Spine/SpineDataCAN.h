#pragma once

#include "SpineData.h"
#include "DataLayerCAN_Fwd.h"

enum Buttons{
    Top_Left_Button = 0,
    Top_Right_Button,
    Bottom_Left_Button,
    Bottom_Right_Button
};

class SpineDataCAN : public SpineData
{
public:

	SpineDataCAN();
    
	bool					SetReceivedDataToObject(DataLayerCAN* dataLayer);
    void                    GetActualMotorSpeeds(int *motor1, int *motor2, int *motor3);
    bool                    GetStopButton() { return buttonPresses_[Bottom_Left_Button]; };
    bool                    GetRightUpButton() { return buttonPresses_[Top_Right_Button]; };
    bool                    GetRightBottomButton() { return buttonPresses_[Bottom_Right_Button]; };

    static const int        SerialPacketSize = 13; // 0xAA, id, dlc, 8 bytes data, 2 bytes crc 

    void                    SetDummyData();
private:
	int                     motorSpeed1_;
    int                     motorSpeed2_;
    int                     motorSpeed3_;
    bool                    buttonPresses_[4];
};
