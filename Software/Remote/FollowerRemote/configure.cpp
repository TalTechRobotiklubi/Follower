#include "configure.h"
#include "ui_configure.h"

#include <QDir>
#include <QDebug>

Configure::Configure(QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::Configure)
  , isSending_(false)
  , sendCount_(0)
  , fileStream_(0)
{
  ui->setupUi(this);
  initTable();
  connect(&timer_, &QTimer::timeout, this, &Configure::send);
}

Configure::~Configure()
{
  if (activeFile_.isOpen())
    activeFile_.close();
  delete fileStream_;
  delete ui;
}

void Configure::onNewFeedbackData(const QList<uint8_t>& list)
{
  if (activeFile_.isOpen())
  {
    foreach(const uint8_t d, list)
    {
      *fileStream_ << d;
      *fileStream_ << ",";
    }
    *fileStream_ << "\r\n";
  }
}

void Configure::on_button_send_clicked()
{
  if(!isSending_ && !timer_.isActive())
  {
    sendCount_ = 0;
    if (activeFile_.isOpen())
      activeFile_.close();

    activeFile_.setFileName(createFilePath());
    if (activeFile_.open(QFile::ReadWrite))
      writePidParametersToFile();
    else
      qDebug() << "Measuremnt file open failed!";
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

QString Configure::createFilePath()
{
  const QString fileStart = "measure";
  QStringList nameFilter("*.csv");
  QDir directory(QDir::currentPath().append(QDir::separator()).append("measurements"));
  QStringList files = directory.entryList(nameFilter);
  QString newFile = fileStart;
  if (files.size() == 0)
    newFile.append("000.csv");
  else
  {
    int lastNum = files.last().mid(fileStart.size(), 3).toInt();
    ++lastNum;
    QString lastNumStr;
    if (lastNum < 10)
      lastNumStr.append("00");
    else if (lastNum < 100)
      lastNumStr.append("0");

    lastNumStr.append(QString("%1").arg(lastNum));
    newFile.append(lastNumStr + ".csv");
  }
  return QString(directory.path() + QDir::separator() + newFile);
}

void Configure::writePidParametersToFile()
{
  delete fileStream_;
  fileStream_ = new QTextStream(&activeFile_);
  *fileStream_ << "Parameters\r\n";
  *fileStream_ << "----------\r\n";
  for(int i = 0; i < ui->tableParams->rowCount(); ++i)
  {
    for (int j = 0; j < ui->tableParams->columnCount(); ++j)
      *fileStream_ << ui->tableParams->item(i, j)->text().append(" ").toLatin1();
    *fileStream_ << "\r\n";
  }
  *fileStream_ << "\r\n";
  *fileStream_ << "Measurements\r\n";
  *fileStream_ << "----------\r\n";
}
