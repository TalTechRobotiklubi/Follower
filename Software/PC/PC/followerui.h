#ifndef FOLLOWERUI_H
#define FOLLOWERUI_H

#include <QtGui/QMainWindow>
#include "ui_followerui.h"
#include "Follower.h"
#include "SpineDataCAN.h"

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
    void newUiData(SpineDataCAN* spineData);
};

#endif // FOLLOWERUI_H
