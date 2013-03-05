#ifndef FOLLOWERUI_H
#define FOLLOWERUI_H

#include <QtGui/QMainWindow>
#include "ui_followerui.h"
#include "Follower.h"

class FollowerUi : public QMainWindow
{
    Q_OBJECT

public:
    explicit FollowerUi(Follower *robot);
    FollowerUi(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~FollowerUi();

private:
    Ui::FollowerUiClass ui;
    CANWorkerThread*		workerThread_;

private slots:
    void connectSpine();
    void newSensorData(int s1, int s2, int s3, int s4);
};

#endif // FOLLOWERUI_H
