#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QtCharts/QtCharts>
#include "Models/model_include.h"

using namespace QtCharts;

namespace Ui {
class ChartWidget;
}

class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChartWidget(QWidget *parent = 0);
    ~ChartWidget();

    void refreshChart(uint8_t type,float value);
    void clearSerials();

private:
    Ui::ChartWidget *ui;

    QChart *chart;

    QSplineSeries *series1;
    QSplineSeries *series2;
    QSplineSeries *series3;

    void initChart();

};

#endif // CHARTWIDGET_H
