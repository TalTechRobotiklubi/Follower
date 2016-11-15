#ifndef FOLLOWERUI_H
#define FOLLOWERUI_H

#include <stdint.h>

#include <QtWidgets/QMainWindow>
#include "ui_followerui.h"
#include "Follower.h"
#include "TRobot.h"
#include "configure.h"
#include "chartdisplay.h"

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
  virtual void keyPressEvent(QKeyEvent* event) override;
  virtual void keyReleaseEvent(QKeyEvent* event) override;
  virtual void wheelEvent(QWheelEvent* event) override;
  virtual void mousePressEvent(QMouseEvent *event) override;
  virtual void mouseMoveEvent(QMouseEvent *event) override;
  virtual void closeEvent(QCloseEvent * event) override;

signals:
  void startCommunication(QString nPort);
  void stopCommunication(void);
  void feedbackReceived(const QList<int16_t> list);

public slots:
  void connectSpine();
  //  it is called to give feedback about connection success
  void startCommStatus(bool status);
  void stopCommStatus();
  void newUiData();
  void updatePortList();
  void keyboardDisabled(bool disabled);

private:
  void setupPidConfig();
  void calcAndWriteEulerAnglesToUI(int16_t raw_qw, int16_t raw_qx, int16_t raw_qy, int16_t raw_qz);

  Ui::FollowerUiClass ui;
  WorkerThreadBase* workerThread_;
  DataLayerBase* dataLayer_;
  WorkerObjectBase* workerObject_;
  Kinematics* kinematics_;
  TRobot* robotgui_;
  QGraphicsScene* scene_;
  QSettings* settings_;
  Configure* conf_;
  bool keyboardOff_;
  ChartDisplay* charts_;
};

#endif // FOLLOWERUI_H
