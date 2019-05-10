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
#include "Models/model_threadlog.h"

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
    void itemNameChangedSlot(QString name);
    void hardCfgWindowClose();

    void on_about_triggered();

    void on_acttest_triggered(bool checked);

    void UartRxDealSlot(char cmd,uint8_t dLen,char *dat);
    void UartByteArraySlot(QByteArray revDats,uartDir dir,bool isHex);
    void UartRxAckResault(bool result);
    void UartErrorDeal();

    void unitStartExeSlot(tUnit eUnit);
    void unitEndExeSlot(bool exeResult);
    void onEndTestSlot();
    void execKeyClicked(QString);
    void chkParamFromHardware(uint8_t chk);

    void on_treeWidget_uartOpen(const QString &com, const QString &baud);
    void on_treeWidget_uartClose();
    void on_treeWidget_devUseState(bool isUse);

    void on_pushButtonhide_clicked(bool checked);



    void on_btnReadCurrent_clicked();

    void onProcessOverSlot(uint8_t pNum);
    void onProcessOutputSlot(int pNum,QString String);

    void on_actReport_triggered();

    void on_actHelp_triggered();

    void on_actATtool_triggered();

    void on_BtnCircularCurrent_clicked();

    void on_BtnOverCurrent_clicked();

    void on_actPause_triggered(bool checked);

    void on_btnReadVolt_clicked();

    void on_BtnCircularVolt_clicked();

    void on_BtnOverVolt_clicked();

    void on_BtnCircularAudio_clicked();

    void on_btnReadAudio_clicked();

    void on_BtnOverAudio_clicked();

private:
    Ui::MainWindow *ui;

    ChartWidget *chartDeal;
    Model_String *qtStrDeal;


    ResHardware *hardCfg;
    bool hardCfgIsOpen;
    QString gstr_hardCfgCOM;

    QList<QDockWidget*> m_docks;//< 记录所有dockWidget的指针
    void initMainWindow();

    void setIsRunInterface(bool IsRun);

    /*
     * 测试流处理
    */
    typedef enum
    {
        NONE,
        start,
        waitnull,
        overtest,
        getprop,
        report,
        kltime,
    }test_type_e;
    test_type_e testState;
    Model_tFlow *tFlowDeal;
    int timerTestID;
    bool isHadProp;
    bool isHadReport;
    bool isRunning;
    QList <tUnit> *exeFlow;

    void startTheFlow(QList <tUnit> *testFlow);
    void endTheFlow();
    bool getTestRunState();
    void timerTestIDDeal();
    void testProcessOutputDeal(QString String);
    void testProcessOverDeal();

    /*
     * 串口处理
    */
    Model_UART *UARTDeal;

    void initUartParam();
    void deleteUartParam();
    void UartOpen(const QString &com, const QString &baud);
    void UartClose();
    bool UartConnectStatus();


    /*
     * 进程处理
    */
    Model_Process *PRODeal;         //进程处理对象
    bool isPRORunning;              //进程是否正在运行标志
    int timerProID;                 //进程处理定时器ID
    QString currentCMDString;       //当前命令字符串
    QStringList proList;            //进程处理解析字符串列表：用来显示

    void initProcessDeal();
    void deleteProcessDeal();
    void proStopSysLogcat();
    void proStopSysUiautomator();
    bool proSysIsRunning();
    void timerProIDDeal();

    /*
     * 采集Logcat线程处理
    */
    model_ThreadLog *logThreadDeal;
    bool IsLogcatEnable;
    QString logcatPath;

    void initLogcatThreadDeal();
    void deleteLogcatThreadDeal();
    void startLogThread();
    void stopLogThread();


    /*
     * 其他
    */
    int timer1SID;
    QDateTime testTime;
    int clearWindowCountor;
    //运行状态列表
    enum
    {
       colUnit,
       colAct,
       colResult,
       colStr
    };

    int chkParamCount;


protected:
    void closeEvent(QCloseEvent *event);
    void timerEvent(QTimerEvent *event);
};

#endif // MAINWINDOW_H
