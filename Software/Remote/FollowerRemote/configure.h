#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <stdint.h>

#include <QDialog>
#include <QTimer>
#include <QFile>
#include <QTextStream>

namespace Ui {
class Configure;
}

class Configure : public QDialog
{
  Q_OBJECT

public:
  explicit Configure(QWidget *parent = 0);
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

  Ui::Configure *ui;
  QTimer timer_;
  bool isSending_;
  int sendCount_;
  QFile activeFile_;
  QTextStream* fileStream_;
};

#endif // CONFIGURE_H
