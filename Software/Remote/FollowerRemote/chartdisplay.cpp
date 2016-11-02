#include "chartdisplay.h"
#include "ui_chartdisplay.h"

#include <QDebug>
#include <QDir>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

const int ColCount = 6;

ChartDisplay::ChartDisplay(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::chartdisplay)
{
  ui->setupUi(this);
  chart_ = new QChart();

  for (int i = 0; i < ColCount; i++)
  {
    this->logWhitelist_.append(false);
    this->logAssoc_.append(QString("Data") + QString("%1").arg(i + 1));
  }

  this->ui->seriesList->addItems(logAssoc_);

  scene_ = new QGraphicsScene(this);
  scene_->setBackgroundBrush(QBrush(Qt::black));
  ui->graphicsView->setScene(scene_);
  ui->graphicsView->setRenderHint(QPainter::Antialiasing);
}

ChartDisplay::~ChartDisplay()
{
  delete ui;
}

void ChartDisplay::drawChart(const QList<QLineSeries*>& seriesList)
{
  chart_->removeAllSeries();
  chart_->legend()->hide();

  for (int i = 0; i < ColCount; i++)
    chart_->addSeries(seriesList.at(i));

  chart_->createDefaultAxes();
  chart_->setTitle("Test chart");
  chart_->setSizePolicy(ui->graphicsView->sizePolicy());
  chart_->setMinimumSize(ui->graphicsView->size());

  scene_->addItem(chart_);
  scene_->update();
}

void ChartDisplay::readFromFile(QString filePath)
{
  qDebug() << "Two.";
  QFile file(filePath);
  bool atData = false;
  QList<QLineSeries *> seriesList;

  for (int j = 0; j < ColCount; j++)
    seriesList.append(new QLineSeries);

  // Try to open the file.
  if (!file.open(QIODevice::ReadOnly))
  {
    qDebug() << "Error loading chart data from file: " << file.errorString();
    return;
  }

  // Start reading the file.
  int lineIndex = 0;
  while (!file.atEnd())
  {
    QByteArray line = file.readLine().trimmed();

    if (!atData)
    {
      // Measutrements start delimited with this key word.
      // Keep looking until we find it.
      qDebug() << "No data";
      if (line.contains("Measurements"))
      {
        atData = true;
      }
    }
    else
    {
      qDebug() << "Have data";
      // We're here, so we're expecting data. 6 columns to be exact.
      // Save the array, parse it, add it to the respective data series.
      QList<QByteArray> dataList(line.split(','));
      for (int j = 0; j < ColCount; j++)
      {
        QString str(dataList[j].constData());
        seriesList.at(j)->append(lineIndex, str.toFloat());
      }

      lineIndex++;
    }
  }

  if (!atData)
    return;

  drawChart(seriesList);
}

void ChartDisplay::showEvent(QShowEvent* event)
{
  populateFileList();
  QWidget::showEvent(event);
}

void ChartDisplay::redrawLines()
{
  for (int i = 0; i < ColCount; i++)
  {
    chart_->series().at(i)->setVisible(logWhitelist_[i]);
    //currentLog_[i]->setVisible();
  }

  scene_->update();
}

void ChartDisplay::populateFileList()
{
  QDir directory(QDir::currentPath().append(QDir::separator()).append("measurements"));

  QString filter = "*.csv";
  QStringList measureFiles = directory.entryList(filter.split(" "));
  measureFiles.prepend("");

  this->ui->fileSelect->clear();
  this->ui->fileSelect->addItems(measureFiles);
  this->ui->fileSelect->setEditable(false);
}

void ChartDisplay::on_fileSelect_currentIndexChanged(const QString &arg1)
{
  if (arg1.length())
  {
    readFromFile(QDir::currentPath().append(QDir::separator()).append("measurements") + QDir::separator() + arg1);
  }
}

void ChartDisplay::on_seriesList_itemClicked(QListWidgetItem *item)
{
  //    logWhitelist[logAssoc.indexOf(item->text())] = item->isSelected();
  qDebug() << "Three.";

  qDebug() << this->logAssoc_.indexOf(item->text());
  int i = this->logAssoc_.indexOf(item->text());
  qDebug() << this->logWhitelist_.length();
  this->logWhitelist_[i] = !this->logWhitelist_[i];
  qDebug() << this->logWhitelist_[i];
  this->redrawLines();
}
