#pragma once

#include <qtimer.h>
#include <qobject.h>

class DataLayerBase;

class Kinematics : public QObject
{
    Q_OBJECT
public:
    enum Direction
    {
        Forward,
        Backward,
        Right,
        Left,
        Stop
    };

    Kinematics(DataLayerBase* dataLayer);
    ~Kinematics(void);

    void startTimer();
    void stopTimer();

    //void move(Direction direction, int speed);
    void forward(int speed);
    void backward(int speed);
    void right(int speed);
    void left(int speed);
    void stop();
    void startAlgorithm();
    void stopAlgorithm();

public slots:
    void timerUpdate();
private:
    void calculateAndSetSpeeds();
    int calculateNewSpeed(int currentSpeed, int requestedSpeed);

    void runAlgorithm();
    void algorithm1();

    static const int ACCELER_RATE = 100;

    Direction       state_;
    int             requestM1_;
    int             requestM2_;
    int             currentM1_;
    int             currentM2_;
    DataLayerBase*  dataLayer_;
    QTimer          timer_;
    bool            running_;
};

