#include "algorithmwander.h"

#include "DataLayerBase.h"

AlgorithmWander::AlgorithmWander(DataLayerBase* dataLayer)
{
  this->dataLayer = dataLayer;
}

void AlgorithmWander::start()
{
    counter = 0;
    lastState = 0;
    speed = 1200;
}

AlgorithmWander::Execution AlgorithmWander::run()
{
    Execution execution;
    unsigned char sensorLeft;
    unsigned char sensorRight;

    unsigned char sensorDiagLeft;
    unsigned char sensorDiagRight;

    dataLayer->DL_getData(DLParamDistanceSensor7, &sensorLeft);
    dataLayer->DL_getData(DLParamDistanceSensor1, &sensorRight);
    dataLayer->DL_getData(DLParamDistanceSensor8, &sensorDiagLeft);
    dataLayer->DL_getData(DLParamDistanceSensor2, &sensorDiagRight);

    if (((sensorDiagLeft < StopDistance)&&(sensorDiagLeft))
        ||((sensorDiagRight < StopDistance)&&(sensorDiagRight))
        ||((sensorLeft < StopDistance)&&(sensorLeft))
        ||((sensorRight < StopDistance)&&(sensorRight)))
    {
        if (lastState)
        {
            counter++;
        }
        if (counter & 1)
        {
            //motorspeed(900,-900);
            execution.leftSpeed = 900;
            execution.rightSpeed = -900;
        }
        else
        {
            //motorspeed(-900,900);
            execution.leftSpeed = -900;
            execution.rightSpeed = 900;
        }
        lastState = 0;
        pidI = 0;
        pidD = 0;
        pidP = 0;
    }
    else
    {
        pidD = pidP;
        pidP = getSensorError()/25;

        if (pidP > 1000)
            pidP = 1000;

        if (pidP < -1000)
            pidP = -1000;

        pidD = pidP-pidD;

        speed = 1200;//2000-abs(p);

        if ((pidI < 50000)&&(pidI >-50000))
        {
            pidI += pidP;
        }

        int pid = pidP;//+i/75+d/4;

        //motorspeed(speed-pid,speed+pid);
        execution.leftSpeed = speed-pid;
        execution.rightSpeed = speed+pid;
        lastState = 1;
    }
    return execution;
}

void AlgorithmWander::stop()
{

}

int AlgorithmWander::getSensorError()
{
    int errconst[4] = {-50,10,10,50};
    DLParam senslist[4] = {DLParamDistanceSensor8,DLParamDistanceSensor7,DLParamDistanceSensor1,DLParamDistanceSensor2};
    int sum = 0;
    unsigned char sensorvalue;


    for (int i = 0; i<4; i++)
    {
        dataLayer->DL_getData(senslist[i], &sensorvalue);

        if ( sensorvalue == 0 )
        {
            sensorvalue = 255;
        }


        sum += errconst[i]*(255-sensorvalue);
    }

    return sum;
}


