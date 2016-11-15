#include "chartdisplay.h"
#include "ui_chartdisplay.h"

#include <QDebug>
#include <QDir>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

const int ColCount = 6;

ChartDisplay::ChartDisplay(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::chartdisplay),
  chartCreated_(false)
{
  ui->setupUi(this);
  chart_ = new QChart();

  for (int i = 0; i < ColCount; i++)
  {
    this->logWhitelist_.append(false);
    this->logAssoc_.append(QString("Data") + QString("%1").arg(i + 1));
  }

  this->ui->seriesList->addItems(logAssoc_);

  for (int i = 0; i < ColCount; i++)
  {
    QListWidgetItem *item = this->ui->seriesList->item(i);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
  }

  scene_ = new QGraphicsScene(this);
  scene_->setBackgroundBrush(QBrush(Qt::black));
  ui->graphicsView->setScene(scene_);
  ui->graphicsView->setRenderHint(QPainter::Antialiasing);

  connect(this->ui->fileList, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(handleFileUpdate(QListWidgetItem *)));
  connect(this->ui->seriesList, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(handleDataUpdate(QListWidgetItem*)));
}

ChartDisplay::~ChartDisplay()
{
  chart_->removeAllSeries();
  delete ui;
}

void ChartDisplay::drawChart(const QList<QLineSeries*>& seriesList)
{
  chart_->removeAllSeries();

  for (int i = 0; i < seriesList.length(); i++)
  {
    chart_->addSeries(seriesList.at(i));
  }

  chart_->createDefaultAxes();

  if (!chartCreated_)
  {


    chart_->setTitle("Feedback Data");
    chart_->setSizePolicy(ui->graphicsView->sizePolicy());
    chart_->setMinimumSize(ui->graphicsView->size());

    scene_->addItem(chart_);

    chartCreated_ = true;
  }

  scene_->update();
}

QList<QLineSeries *> ChartDisplay::readFromFile(QString fileName)
{
  QFile file(QDir::currentPath().append(QDir::separator()).append("measurements") + QDir::separator() + fileName);
  bool atData = false;
  QList<QLineSeries *> seriesList;

  for (int j = 0; j < ColCount; j++)
  {
    seriesList.append(new QLineSeries);
    seriesList.at(j)->setName(fileName + QString("-#%1").arg(j + 1));
    seriesList.at(j)->setVisible(logWhitelist_[j]);
  }

  // Try to open the file.
  if (!file.open(QIODevice::ReadOnly))
  {
    qDebug() << "Error loading chart data from file: " << file.errorString();
    return seriesList;
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
      if (line.contains("Measurements"))
      {
        atData = true;
      }
    }
    else
    {
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

  return seriesList;
}

void ChartDisplay::showEvent(QShowEvent* event)
{
  populateFileList();
  QWidget::showEvent(event);
}

void ChartDisplay::redrawLines()
{
  for (int i = 0; i < chart_->series().length(); i++)
  {
    chart_->series().at(i)->setVisible(logWhitelist_[i % 6]);
  }

  scene_->update();
}

void ChartDisplay::populateFileList()
{
  this->ui->fileList->clear();

  QDir directory(QDir::currentPath().append(QDir::separator()).append("measurements"));

  QString filter = "*.csv";
  QStringList measureFiles = directory.entryList(filter.split(" "));

  this->ui->fileList->addItems(measureFiles);

  for (int i = 0; i < this->ui->fileList->count(); i++)
  {
    QListWidgetItem *item = this->ui->fileList->item(i);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);
  }
}

void ChartDisplay::handleMultiChart(QStringList fileNames)
{
  if (fileNames.length() == 0)
  {
    chart_->removeAllSeries();
    return;
  }

  QList<QLineSeries *> lineSeries;
  foreach (QString name, fileNames)
  {
    lineSeries.append(readFromFile(name));
  }

  if (lineSeries.length() == 0)
  {
    chart_->removeAllSeries();
    return;
  }

  drawChart(lineSeries);
}

void ChartDisplay::on_updateFiles_clicked()
{
  if (chart_->series().length())
    chart_->removeAllSeries();

  populateFileList();
}

void ChartDisplay::handleFileUpdate(QListWidgetItem *)
{
  QStringList fileNames = *new QStringList;
  for (int i = 0; i < this->ui->fileList->count(); i++)
  {
    QListWidgetItem *item = this->ui->fileList->item(i);
    if (item->checkState() == Qt::Checked)
    {
      fileNames.append(item->text());
    }
  }

  handleMultiChart(fileNames);
}

void ChartDisplay::handleDataUpdate(QListWidgetItem *item)
{
  int i = this->logAssoc_.indexOf(item->text());
  this->logWhitelist_[i] = item->checkState() == Qt::Checked;

  this->redrawLines();
}
