#include "chartdisplay.h"
#include "ui_chartdisplay.h"

#include <QDebug>
#include <QDir>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#define COL_COUNT 6

ChartDisplay::ChartDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chartdisplay)
{
    qDebug() << "One";
    ui->setupUi(this);

    QDir directory(QDir::currentPath().append(QDir::separator()).append("measurements"));

    QString filter = "*.csv";
    QStringList measureFiles = directory.entryList(filter.split(" "));
    measureFiles.prepend("");

    this->ui->fileSelect->addItems(measureFiles);
    this->ui->fileSelect->setEditable(false);

    for (int i = 0; i < COL_COUNT; i++)
    {
        this->logWhitelist.append(false);
        this->logAssoc.append(QString("Data") + QString("%1").arg(i + 1));
    }

    this->ui->seriesList->addItems(logAssoc);

    scene_ = new QGraphicsScene(this);
    scene_->setBackgroundBrush(QBrush(Qt::black));
    ui->graphicsView->setScene(scene_);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
}

ChartDisplay::~ChartDisplay()
{
    delete ui;
}

void ChartDisplay::ReadFromFile(QString filePath)
{
    qDebug() << "Two.";
    QFile file(filePath);
    bool atData = false;
    int i = 0;

    // Initialize the line series in the log array.
    currentLog.clear();

    for (int j = 0; j < COL_COUNT; j++)
    {
        currentLog.append(new QLineSeries);
    }

    // Try to open the file.
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Error loading chart data from file: " << file.errorString();
        return;
    }

    // Start reading the file.
    while (!file.atEnd())
    {
        QByteArray line = file.readLine();

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
            for (int j = 0; j < COL_COUNT; j++)
            {
                QString str(dataList[j].constData());
                currentLog[j]->append(i, str.toFloat());
            }

            i++;
        }
    }

    if (!atData)
    {
        return;
    }

    this->chart_ = new QChart();
    chart_->legend()->hide();

    for (i = 0; i < COL_COUNT; i++)
    {
        chart_->addSeries(currentLog[i]);
    }

    chart_->createDefaultAxes();
    chart_->setTitle("Test chart");
    chart_->setSizePolicy(ui->graphicsView->sizePolicy());
    chart_->setMinimumSize(ui->graphicsView->size());

    scene_->addItem(chart_);
    scene_->update();
    qDebug() << "Got here weee";
}

void ChartDisplay::RedrawLines()
{
    for (int i = 0; i < COL_COUNT; i++)
    {
        currentLog[i]->setVisible(logWhitelist[i]);
    }

    scene_->update();
}

void ChartDisplay::on_fileSelect_currentIndexChanged(const QString &arg1)
{
    if (arg1.length())
    {
        ReadFromFile(QDir::currentPath().append(QDir::separator()).append("measurements") + QDir::separator() + arg1);
    }
}

void ChartDisplay::on_seriesList_itemClicked(QListWidgetItem *item)
{
//    logWhitelist[logAssoc.indexOf(item->text())] = item->isSelected();
    qDebug() << "Three.";

    qDebug() << this->logAssoc.indexOf(item->text());
    int i = this->logAssoc.indexOf(item->text());
    qDebug() << this->logWhitelist.length();
    this->logWhitelist[i] = !this->logWhitelist[i];
    qDebug() << this->logWhitelist[i];
    this->RedrawLines();
}
