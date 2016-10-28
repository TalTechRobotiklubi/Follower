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
    ui->setupUi(this);

    QDir directory(QDir::currentPath().append(QDir::separator()).append("measurements"));

    /*
     * Uncomment if you want to read files by hand.
    if (!directory.exists())
    {
      directory.mkdir(directory.absolutePath());
    }

    ReadFromFile(directory.path() + QDir::separator() + "test1.csv");
     */
}

ChartDisplay::~ChartDisplay()
{
    delete ui;
}

void ChartDisplay::ReadFromFile(QString filePath)
{
    qDebug() << "Started.";
    QFile file(filePath);
    bool atData = false;
    int i = 0;

    // Initialize the line series in the log array.
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

    QChart *chart = new QChart();
    chart->legend()->hide();

    for (i = 0; i < COL_COUNT; i++)
    {
        chart->addSeries(currentLog[i]);
    }

    chart->createDefaultAxes();
    chart->setTitle("Test chart");

    QChartView *chartView = new QChartView(chart, this);
    chartView->setMinimumSize(this->size());
    chartView->setRenderHint(QPainter::Antialiasing);
}
