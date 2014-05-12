#ifndef FOLLOWERUI_H
#define FOLLOWERUI_H

#include <QtWidgets/QMainWindow>
#include "ui_followerui.h"
#include "Follower.h"
#include "TRobot.h"

class DataLayerBase;
class WorkerObjectBase;
class Kinematics;

class FollowerUi : public QMainWindow
{
    Q_OBJECT

public:
    explicit FollowerUi(Follower *robot);
    FollowerUi(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~FollowerUi();

protected:
    virtual void keyPressEvent ( QKeyEvent * event );
	virtual void wheelEvent (QWheelEvent* event);
	virtual void mousePressEvent(QMouseEvent *event);
	//void mouseReleaseEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);

signals:
    void startCommunication(void);
    void stopCommunication(void);
    void newCommands(void);

public slots:
    void connectSpine();
    //  it is called to give feedback about connection success
    void startCommStatus(bool status);
    void stopCommStatus(bool status);
    //void newUiData(SpineData* spineData);
    void newUiData();

private:
    void sendCmd( int w1, int  w2, int w3 );
 
    Ui::FollowerUiClass     ui;
    WorkerThreadBase*		workerThread_;
    DataLayerBase*          dataLayer_;
    WorkerObjectBase*       workerObject_;
    Kinematics*             kinematics_;
	TRobot*                 robotgui_;
	QGraphicsScene*         scene_;



};

#endif // FOLLOWERUI_H