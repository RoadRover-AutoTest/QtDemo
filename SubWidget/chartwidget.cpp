#include "chartwidget.h"
#include "ui_chartwidget.h"

ChartWidget::ChartWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChartWidget)
{
    ui->setupUi(this);

    //初始化QChart
    initChart();
}

ChartWidget::~ChartWidget()
{
    delete series2;
    delete series1;
    delete chart;
    delete ui;
}

/*************************************************************
/函数功能：刷新数据
/函数参数：数据类型  数据值
/函数返回：wu
*************************************************************/
void ChartWidget::refreshChart(uint8_t type,float value)
{
    //每增加一个点改变X轴的范围，实现曲线的动态更新效果
    chart->axisX()->setMin(QDateTime::currentDateTime().addSecs(-60*1));
    chart->axisX()->setMax(QDateTime::currentDateTime().addSecs(0));

    //当曲线上最早的点超出X轴的范围时，剔除最早的点，
    if(series1->count()>119)
    {
        series1->removePoints(0,series1->count()-119);
    }

    if(series2->count()>119)
    {
        series2->removePoints(0,series2->count()-119);
    }

    if(type == CHKCurrent)
    {
        series1->append(QDateTime::currentDateTime().toMSecsSinceEpoch(),value);//增加新的点到曲线末端
    }
    if(type & CHKVlot)
    {
        series2->append(QDateTime::currentDateTime().toMSecsSinceEpoch(),value);//增加新的点到曲线末端
    }

    ui->lineEdit_Cur->setText(QString::number(value));
}

/*************************************************************
/函数功能：实现QChart的初始化函数
/函数参数：无
/函数返回：wu
*************************************************************/
void ChartWidget::initChart()
{
    //初始化QChart的实例
    chart=new QChart();

    //初始化两个QSplineSeries的实例
    series1=new QSplineSeries();
    series2=new QSplineSeries();
    series1->setName("Current(A)");
    series2->setName("Volt(V)");
    chart->addSeries(series1);
    chart->addSeries(series2);

    //声明并初始化X轴、两个Y轴
    QDateTimeAxis *axisX=new QDateTimeAxis();
    QValueAxis *axisY_1=new QValueAxis();
    QValueAxis *axisY_2=new QValueAxis();

    //设置坐标轴显示的范围
    axisX->setMin(QDateTime::currentDateTime().addSecs(-60*1));
    axisX->setMax(QDateTime::currentDateTime().addSecs(0));
    axisY_1->setMin(0);
    axisY_1->setMax(2);
    axisY_2->setMin(0);
    axisY_2->setMax(18);

    //设置坐标轴上的格点
    axisY_1->setTickCount(20);
    axisY_2->setTickCount(18);

    //设置坐标轴显示的名称
    axisX->setTitleText("X轴");
    axisY_1->setTitleText("axisY_Current");
    axisY_2->setTitleText("axisY_Volt");

    //设置坐标轴的颜色，粗细，设置网格不显示
    axisY_1->setLinePenColor(QColor(Qt::darkBlue));
    axisY_1->setGridLineColor(QColor(Qt::darkBlue));
    axisY_2->setLinePenColor(QColor(Qt::darkGreen));
    axisY_2->setGridLineColor(QColor(Qt::darkGreen));
    axisY_1->setGridLineVisible(false);
    axisY_2->setGridLineVisible(false);

    QPen penY1(Qt::darkBlue,1,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);
    QPen penY2(Qt::darkGreen,1,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin);

    axisY_1->setLinePen(penY1);
    axisY_2->setLinePen(penY2);

    //把坐标轴添加到chart中，
    //addAxis函数的第二个参数是设置坐标轴的位置，
    //只有四个选项，下方：Qt::AlignBottom，左边：Qt::AlignLeft，右边：Qt::AlignRight，上方：Qt::AlignTop
    chart->addAxis(axisX,Qt::AlignBottom);
    chart->addAxis(axisY_1,Qt::AlignLeft);
    chart->addAxis(axisY_2,Qt::AlignRight);

    //把曲线关联到坐标轴
    series1->attachAxis(axisX);
    series1->attachAxis(axisY_1);
    series2->attachAxis(axisX);
    series2->attachAxis(axisY_2);

    //把chart显示到窗口上
    ui->graphicsView->setChart(chart);
}
