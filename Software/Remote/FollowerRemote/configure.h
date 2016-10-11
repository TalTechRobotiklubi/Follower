#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <stdint.h>

#include <QDialog>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QSettings>

#include "Kinematics.h"

namespace Ui {
class Configure;
}

class Configure : public QDialog
{
  Q_OBJECT

public:
  explicit Configure(QSettings *settings, Kinematics *kinematics, QWidget *parent = 0);
  ~Configure();

signals:
  void sendParameter(uint8_t param, float value);
  void parameterSendFinished();
  void routineStatus(bool running);

public slots:
  void onNewFeedbackData(const QList<uint8_t>& list);

private slots:
  void on_button_send_clicked();
  void on_button_log_clicked();
  void on_tableTest_cellChanged(int row);
  void on_button_test_clicked();

private:
  void initTableParams();
  void initTableTesting();
  void addParameterToTable(const QString& param, int row, float value);
  void send();
  QString createFilePath();
  void writePidParametersToFile();
  void savePidParameters();
  int loadPidParameters();
  int startLogging();
  void stopLogging();

  void saveTestingParameters();
  int loadTestingParameters();
  int startTesting();
  void stopTesting();
  int iterateTest();

  Ui::Configure *ui;
  QTimer timer_;
  bool isSending_;
  int sendCount_;
  QFile activeFile_;
  QTextStream* fileStream_;
  QSettings* settings_;
  bool isTesting_;
  int currentTestRow_;
  QTimer timerTesting_;
  Kinematics* kinematics_;
};

#endif // CONFIGURE_H
