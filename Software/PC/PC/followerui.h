#ifndef FOLLOWERUI_H
#define FOLLOWERUI_H

#include <QtGui/QMainWindow>
#include "ui_followerui.h"
#include "Follower.h"
#include "SpineDataCAN.h"
#include "TRobot.h"

class FollowerUi : public QMainWindow
{
    Q_OBJECT

public:
    explicit FollowerUi(Follower *robot);
    FollowerUi(QWidget *parent = 0, Qt::WFlags flags = 0);
    ~FollowerUi();

protected:
    virtual void keyPressEvent ( QKeyEvent * event );

private:
    void sendCmd( int w1, int  w2, int w3 );

    Ui::FollowerUiClass ui;
    CANWorkerThread*		workerThread_;
	TRobot *robotgui;
	QGraphicsScene *scene;

private slots:
    void connectSpine();
    void newUiData(SpineData* spineData);
};

#endif // FOLLOWERUI_H
