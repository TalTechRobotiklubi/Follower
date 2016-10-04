#include "configure.h"
#include "ui_configure.h"

#include <QDir>
#include <QSettings>
#include <QDebug>

Configure::Configure(QSettings *settings, QWidget *parent)
  : QDialog(parent)
  , ui(new Ui::Configure)
  , isSending_(false)
  , sendCount_(0)
  , fileStream_(0)
{
  settings_ = settings;
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

    savePidParameters();

    timer_.start(50);
  }
}

void Configure::initTable()
{
  ui->tableParams->setColumnCount(2);

  if (!loadPidParameters())
  {
    ui->tableParams->setRowCount(6);
    addParameterToTable("Px", 0, 0.2f);
    addParameterToTable("Ix", 1, 0.0f);
    addParameterToTable("Dx", 2, 0.0f);
    addParameterToTable("Pw", 3, 0.0f);
    addParameterToTable("Iw", 4, 0.0f);
    addParameterToTable("Dw", 5, 0.0f);
  }

  ui->tableParams->insertRow(ui->tableParams->rowCount());
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
    if (QTableWidgetItem *item = ui->tableParams->item(sendCount_, 1))
    {
        float value = item->text().toFloat();
        emit sendParameter(item->row(),value);
    }
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

  // Make the directory if none exists.
  if (!directory.exists())
  {
    directory.mkdir(directory.absolutePath());
  }

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

  QString filename(directory.path() + QDir::separator() + newFile);
  ui->lbl_filename->setText(directory.relativeFilePath(filename));

  return filename;
}

void Configure::writePidParametersToFile()
{
  fileStream_ = new QTextStream(&activeFile_);
  *fileStream_ << "Parameters\r\n";
  *fileStream_ << "----------\r\n";
  for(int i = 0; i < ui->tableParams->rowCount(); ++i)
  {
    for (int j = 0; j < ui->tableParams->columnCount(); ++j)
      if (QTableWidgetItem *item = ui->tableParams->item(i, j))
        *fileStream_ << item->text().append(" ").toLatin1();
    *fileStream_ << "\r\n";
  }
  *fileStream_ << "\r\n";
  *fileStream_ << "Measurements\r\n";
  *fileStream_ << "----------\r\n";
}

void Configure::savePidParameters()
{
  settings_->beginGroup("pidParams");
  settings_->beginWriteArray("parameter");

  for (int i = 0; i < ui->tableParams->rowCount(); i++)
  {
    QTableWidgetItem *itemKey = ui->tableParams->item(i, 0);
    QTableWidgetItem *itemValue = ui->tableParams->item(i, 1);

    if (itemKey && itemValue)
    {
        settings_->setArrayIndex(i);
        settings_->setValue("key", itemKey->text());
        settings_->setValue("value", itemValue->text());
    }
  }

  settings_->endArray();
  settings_->endGroup();
}

int Configure::loadPidParameters()
{
  int size;

  settings_->beginGroup("pidParams");
  size = settings_->beginReadArray("parameter");

  if (!size)
  {
    settings_->endArray();
    settings_->endGroup();

    return 0;
  }

  ui->tableParams->setRowCount(size);

  for (int i = 0; i < size; i++)
  {
    settings_->setArrayIndex(i);

    addParameterToTable(settings_->value("key").toString(), i, settings_->value("value").toFloat());
  }

  settings_->endArray();
  settings_->endGroup();

  return 1;
}

void Configure::on_button_log_clicked()
{
  if (ui->button_log->isChecked())
  {
    // Turn recording on.
    if (startLogging())
    {
      ui->button_log->setText("Stop logging");
    }
    else
    {
      ui->button_log->setChecked(false);
      qDebug() << "Error: Configure failed to start logging.";
    }
  }
  else
  {
    // Turn recording off.
    stopLogging();
    ui->button_log->setText("Start logging");
  }
}

// Starts logging feedback to a new file.
int Configure::startLogging()
{
  savePidParameters();

  if (activeFile_.isOpen())
    activeFile_.close();

  activeFile_.setFileName(createFilePath());
  if (activeFile_.open(QFile::ReadWrite))
  {
    writePidParametersToFile();
  }
  else
  {
    return 0;
  }

  return 1;
}

// Stops the logging of feedback.
void Configure::stopLogging()
{
  activeFile_.close();
  delete fileStream_;
  fileStream_ = nullptr;
}
