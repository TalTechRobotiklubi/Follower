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
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void closeEvent(QCloseEvent * event);

signals:
  void startCommunication(QString nPort);
  void stopCommunication(void);

public slots:
  void connectSpine();
  //  it is called to give feedback about connection success
  void startCommStatus(bool status);
  void stopCommStatus();
  void newUiData();
  void UpdatePortList();

private slots:
  void on_pushButton_clicked();

private:
  Ui::FollowerUiClass ui;
  WorkerThreadBase* workerThread_;
  DataLayerBase* dataLayer_;
  WorkerObjectBase* workerObject_;
  Kinematics* kinematics_;
  TRobot* robotgui_;
  QGraphicsScene* scene_;



};

#endif // FOLLOWERUI_H
