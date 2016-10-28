#ifndef CHARTDISPLAY_H
#define CHARTDISPLAY_H

#include <QWidget>
#include <QList>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
class chartdisplay;
}

class ChartDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit ChartDisplay(QWidget *parent = 0);
    ~ChartDisplay();

    void ReadFromFile(QString filePath);

private:
    Ui::chartdisplay *ui;

    QList<QLineSeries *> currentLog;
};

#endif // CHARTDISPLAY_H
