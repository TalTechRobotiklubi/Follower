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
    void forward(int speed);
    void backward(int speed);
    void right(int speed);
    void left(int speed);
    void stop();
    void startAlgorithm(int algorithmNum);
    void stopAlgorithm();

    void cameraLook(int x, int z);
    void updateFromUi(int translationSpeed,int rotationSpeed, bool smcon);

public slots:
    void timerUpdate();
private:
    void setMotorSpeeds(int speedleft,int speedright);
    void calculateAndSetSpeeds();
    int calculateNewSpeed(int currentSpeed, int requestedSpeed);
    void runAlgorithm();

    static const int ACCELER_RATE = 75;

    int             requestM1_;
    int             requestM2_;
    int             currentM1_;
    int             currentM2_;
    DataLayerBase*  dataLayer_;
    QTimer          timer_;
    bool            running_;
    IAlgorithm*     algorithm_;

    char            currentCamX;
    char            currentCamZ;

    char            currentCamSpeedX;
    char            currentCamspeedZ;

    bool            smoothControl;
    int             k_translationSpeed;
    int             k_rotationSpeed;
};

