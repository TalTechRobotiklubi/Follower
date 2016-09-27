#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <stdint.h>

#include <QDialog>
#include <QTimer>
#include <QFile>
#include <QTextStream>
#include <QSettings>

namespace Ui {
class Configure;
}

class Configure : public QDialog
{
  Q_OBJECT

public:
  explicit Configure(QSettings *settings, QWidget *parent = 0);
  ~Configure();

signals:
  void sendParameter(uint8_t param, float value);
  void parameterSendFinished();

public slots:
  void onNewFeedbackData(const QList<uint8_t>& list);

private slots:
  void on_button_send_clicked();

private:
  void initTable();
  void addParameterToTable(const QString& param, int row, float value);
  void send();
  QString createFilePath();
  void writePidParametersToFile();
  void savePidParameters();
  int loadPidParameters();

  Ui::Configure *ui;
  QTimer timer_;
  bool isSending_;
  int sendCount_;
  QFile activeFile_;
  QTextStream* fileStream_;
  QSettings* settings_;
};

#endif // CONFIGURE_H
