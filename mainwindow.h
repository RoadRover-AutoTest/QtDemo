#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>


#include "Models/model_include.h"
#include "Models/model_uart.h"
#include "Models/model_inisetting.h"
#include "Models/model_tflow.h"
#include "Models/model_string.h"
#include "Models/model_xmlfile.h"
#include "Models/model_process.h"

#include "SubWidget/reshardware.h"
#include "SubWidget/chartwidget.h"
#include "SubWidget/diareport.h"
#include "SubWidget/toolconfig.h"

#define dockMax  5  //5个窗口

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void reRankDockWidget();
    void on_actHard_triggered();

    void on_about_triggered();

    void on_acttest_triggered(bool checked);

    void UartRxDealSlot(char cmd,uint8_t dLen,char *dat);
    void UartByteArraySlot(QByteArray revDats,uartDir dir,bool isHex);
    void UartRxAckResault(bool result);
    void UartDisconnect();

    void unitStartExeSlot(tUnit eUnit);
    void unitEndExeSlot(bool exeResult);
    void onEndTestSlot();
    void execKeyClicked(QString);
    void chkParamFromHardware(uint8_t chk);

    void on_treeWidget_uartOpen(const QString &com, const QString &baud);
    void on_treeWidget_uartClose();
    void on_treeWidget_canOpen(const QString &type, const QString &baud);
    void on_treeWidget_canClose();

    void on_pushButton_2_clicked();

    void on_pushButtonhide_clicked(bool checked);



    void on_btnReadCurrent_clicked();

    void onProcessOverSlot(uint8_t pNum);
    void onProcessOutputSlot(int pNum,QString String);

    void on_actReport_triggered();

    void on_actHelp_triggered();

    void on_actATtool_triggered();

    void on_treeWidget_devUseState(bool isUse);

private:
    Ui::MainWindow *ui;

    ChartWidget *chartDeal;

    QString seqPath;

    QList<QDockWidget*> m_docks;//< 记录所有dockWidget的指针
    void initMainWindow();

    void setIsRunInterface(bool IsRun);
    void initkeyList();

    void startTheFlow(QList <tUnit> *testFlow);
    void endTheFlow();
    bool getTestRunState();
    void timerTestIDDeal();
    void testProcessOutputDeal(QString String);
    void testProcessOverDeal();

    void initUartParam();
    void deleteUartParam();
    void timerUartIDDeal();
    void UartOpen(const QString &com, const QString &baud);
    void UartClose();
    bool UartConnectStatus();

    void initProcessDeal();
    void deleteProcessDeal();
    void proStopSysLogcat();
    void proStopSysUiautomator();
    bool proSysIsRunning();
    void timerProIDDeal();

    void treeWidget_refreshDevNum(QString String);


    int timer1SID;

    Model_UART *UARTDeal;
    QList <uartFrame> txList;      //将待发送的命令填充列表，然后在定时器中调用发送
    int timerUartID;
    uint8_t ackWait;

    void appendTxList(char cmd,char* dat,char len,uint8_t ack);

    Model_tFlow *tFlowDeal;

    Model_String *qtStrDeal;
    Model_Process *PRODeal;
    bool isPRORunning;
    int timerProID;
    QString currentCMDString;

    int timerTestID;

    QDateTime testTime;

    QStringList deviceList;

    bool isDelayReport;//是否延时生成报告：结束测试时有其他进程正在执行将生成报告
    bool isProOK;

    QStringList proList;            //进程处理解析字符串列表：用来显示


    //运行状态列表
    enum
    {
       colUnit,
       colAct,
       colResult,
       colStr
    };

    bool isRunning;


    typedef enum
    {
        start,
        waitnull,
        overtest,
        getprop,
        report,
    }test_type_e;
    test_type_e testState;


protected:
    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);
};

#endif // MAINWINDOW_H
