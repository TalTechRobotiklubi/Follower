#include "algorithmturn.h"

#include "DataLayerBase.h"
#include <QDebug>

AlgorithmTurn::AlgorithmTurn(DataLayerBase *dataLayer)
{
    this->dataLayer = dataLayer;
}


void time_delay() {
    for(int i = 0; i < 1000; i++)
    {
    }
}

void AlgorithmTurn::start()
{

}

AlgorithmTurn::Execution AlgorithmTurn::run() {

    Execution execution;
    unsigned char sensorLeft;
    unsigned char sensorRight;

    unsigned char sensorDiagLeft;
    unsigned char sensorDiagRight;
    dataLayer->DL_getData(DLParamDistanceSensor7, &sensorLeft);
    dataLayer->DL_getData(DLParamDistanceSensor1, &sensorRight);
    dataLayer->DL_getData(DLParamDistanceSensor8, &sensorDiagLeft);
    dataLayer->DL_getData(DLParamDistanceSensor2, &sensorDiagRight);

    /*if (Turn == 200 || Turn == -200) {
        RightAngle = 1;
        Turn = 0;
    }*/



    if (((Turn < -150) && (sensorDiagRight < SafeTurn) && (sensorDiagRight)) ||
       ((Turn > 150) && (sensorDiagLeft < SafeTurn) && (sensorDiagLeft))) {
          execution.xSpeed = speed;
          execution.wSpeed = speed;
          FollowsObstacle = 1;
    }
    else if (((sensorDiagLeft < StopDistance)&&(sensorDiagLeft))
         ||((sensorDiagRight < StopDistance)&&(sensorDiagRight))
         ||((sensorLeft < StopDistance)&&(sensorLeft))
         ||((sensorRight < StopDistance)&&(sensorRight))) {
        if (((sensorDiagRight > sensorDiagLeft) && (sensorDiagLeft) && (sensorDiagRight)) ||
                ((sensorLeft < StopDistance) && (sensorLeft))) {
           execution.xSpeed = 0;
           execution.wSpeed = 0;
           time_delay();
           if (((sensorLeft < StopDistance) && (sensorLeft)) ||
               ((sensorDiagLeft < StopDistance) && (sensorDiagLeft))) {
               execution.xSpeed = 600;
               execution.wSpeed = 0;
               time_delay();
               Turn++;
           }
           else if ((sensorRight < StopDistance) && (sensorRight)) {
               execution.xSpeed = 0;
               execution.wSpeed = 600;
               time_delay();
               Turn--;
           }
        }
        else if (((sensorDiagLeft > sensorDiagRight) && (sensorDiagLeft) && (sensorDiagRight)) ||
                 ((sensorRight < StopDistance) && (sensorRight)))
        {
            execution.xSpeed = 0;
            execution.wSpeed = 0;
            time_delay();
            if (((sensorRight < StopDistance) && (sensorRight)) ||
               ((sensorDiagRight < StopDistance) && (sensorDiagRight))) { //kontroll,et pööre on sooritatud piisavalt
                execution.xSpeed = 0;
                execution.wSpeed = 600;
                time_delay();
                Turn--;
            }
            else if ((sensorLeft < StopDistance) && (sensorLeft)) {
                execution.xSpeed = 600;
                execution.wSpeed = 0;
                time_delay();
                Turn++;
            }
        }
    }
    else if ((Turn < -10) || (Turn > 10))
         {
            if (Turn < 0) {
                execution.xSpeed = 600;
                execution.wSpeed = 0;
                time_delay();
                Turn++;
            }
            else if (Turn > 0) {
                execution.xSpeed = 0;
                execution.wSpeed = 600;
                time_delay();
                Turn--;
            }
    }
    else {
          execution.xSpeed = speed;
          execution.wSpeed = speed;
        }
    
    qDebug() << "T" << Turn << "FO" << FollowsObstacle;
    
    return execution;
}

void AlgorithmTurn::stop()
{

}
