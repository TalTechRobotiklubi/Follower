#ifndef CHARTDISPLAY_H
#define CHARTDISPLAY_H

#include <QDir>
#include <QWidget>
#include <QList>
#include <QListWidgetItem>
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

private slots:
    void on_fileSelect_currentIndexChanged(const QString &arg1);
    void on_seriesList_itemClicked(QListWidgetItem *item);

private:
    void RedrawLines();
    Ui::chartdisplay *ui;

    QList<QLineSeries *> currentLog;
    QList<bool> logWhitelist;
    QStringList logAssoc;
    QGraphicsScene *scene_;
    QChart *chart_;
};

#endif // CHARTDISPLAY_H
