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

    void readFromFile(QString filePath);

protected:
    void showEvent(QShowEvent* event);

private slots:
    void on_fileSelect_currentIndexChanged(const QString &arg1);
    void on_seriesList_itemClicked(QListWidgetItem *item);

private:
    void redrawLines();
    void populateFileList();
    void drawChart(const QList<QLineSeries*>& seriesList);

    Ui::chartdisplay *ui;
    QList<bool> logWhitelist_;
    QStringList logAssoc_;
    QGraphicsScene *scene_;
    QChart *chart_;
};

#endif // CHARTDISPLAY_H
