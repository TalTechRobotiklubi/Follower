#pragma once

#include <QtCore/qtimer.h>
#include <qobject.h>

class DataLayerBase;
class IAlgorithm;

class Kinematics : public QObject
{
    Q_OBJECT
public:
    Kinematics(DataLayerBase* dataLayer);
    ~Kinematics(void);

    void startTimer();
    void stopTimer();
    int translationSpeed();
    int rotationSpeed();
    void stop();
    void startAlgorithm(int algorithmNum);
    void stopAlgorithm();

    void cameraLook(int x, int z);
    void setSpeeds(int translationSpeed,int rotationSpeed);

public slots:
    void timerUpdate();

private:
    void setMotorSpeeds(int speedleft,int speedright);
    void calculateAndSetSpeeds();
    int calculateNewSpeed(int currentSpeed, int requestedSpeed);
    void runAlgorithm();

    static const int ACCELER_RATE = 75;

    DataLayerBase*  dataLayer_;
    QTimer          timer_;
    bool            running_;
    IAlgorithm*     algorithm_;

    char            currentCamX;
    char            currentCamZ;

    char            currentCamSpeedX;
    char            currentCamspeedZ;

    int             translationSpeed_;
    int             rotationSpeed_;
};

