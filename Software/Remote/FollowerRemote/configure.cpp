#include "configure.h"
#include "ui_configure.h"

Configure::Configure(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Configure),
  isSending_(false),
  sendCount_(0)
{
  ui->setupUi(this);
  initTable();
  connect(&timer_, &QTimer::timeout, this, &Configure::send);
}

Configure::~Configure()
{
  delete ui;
}

void Configure::onNewFeedbackData(const QList<uint8_t>& list)
{

}

void Configure::on_button_send_clicked()
{
  if(!isSending_ && !timer_.isActive())
  {
    sendCount_ = 0;
    timer_.start(100);
  }
}

void Configure::initTable()
{
  ui->tableParams->setRowCount(3);
  ui->tableParams->setColumnCount(2);

  addParameterToTable("P", 0, 0.2f);
  addParameterToTable("I", 1, 0.3f);
  addParameterToTable("D", 2, 0.4f);
}

void Configure::addParameterToTable(const QString& param, int row, float value)
{
  QTableWidgetItem *pItem = new QTableWidgetItem(param);
  ui->tableParams->setItem(row, 0, pItem);
  QTableWidgetItem *iItem = new QTableWidgetItem(QString("%1").arg(value));
  ui->tableParams->setItem(row, 1, iItem);
}

void Configure::send()
{
  if (ui->tableParams->rowCount() > 0 && sendCount_ <= ui->tableParams->rowCount())
  {
    QTableWidgetItem *item = ui->tableParams->item(sendCount_, 1);
    float value = item->text().toFloat();
    emit sendParameter(item->row(),value);
    sendCount_++;
  } 
  if (sendCount_ == ui->tableParams->rowCount())
  {
    timer_.stop();
    isSending_ = false;
    emit parameterSendFinished();
  }
}
