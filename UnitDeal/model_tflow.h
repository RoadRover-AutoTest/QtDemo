#ifndef MODEL_TFLOW_H
#define MODEL_TFLOW_H

#include <QObject>
#include <QTimerEvent>
#include <QDebug>
#include "Models/model_include.h"
#include "model_tunit.h"

class Model_tFlow:public QObject
{
    Q_OBJECT
public:
    Model_tFlow(QList <tUnit> *testflow);
    ~Model_tFlow();

    int getTheFlowLoop();
    int getTheUnitLoop();

    void endTheTest();

    void proAppendSysCMD(QString str_ProCmd);

private:
    Model_tUnit *unitDeal;
    QList <tUnit> *dat_theTestFlow;    //测试流
    tUnit dat_theUnit;

    int timer_theFlowID;      //测试单元及测试流执行定时器
    int reTime;//最长1分钟
    int unitIndex;
    int cnt_theFlowCycle;
    bool isUnitDeal;


    typedef enum
    {
        none,
        start,
        dealunit,
        waitnull,
        waitrecover,
        unitover,
        over,
        endflow
    }flow_type_e;

    flow_type_e sta_tflowRunning;
    flow_type_e lastState;




    bool chkWorkState();
    void theFlowReturnUnitDeal();


    Model_Process *mdl_PRODeal;         //进程处理对象
    int timer_ProID;                 //进程处理定时器ID
    bool is_PRORunning;              //进程是否正在运行标志
    QString str_ProCMDDat;       //当前命令字符串
    QStringList lst_ProCMDDat;            //进程处理解析字符串列表：用来显示

    void initProcessDeal();
    void deleteProcessDeal();
    void proStopSysLogcat();
    void proStopSysUiautomator();
    bool proSysIsRunning();
    void timerProIDDeal();


private slots:
    void onStartTheUnitSlot();
    void onEndTheUnitSlot();
    void onUnitResultSlot(bool result);

    //void onProcessOverSlot(uint8_t pNum);
    //void onProcessOutputSlot(int pNum,QString String);


protected:
    void timerEvent(QTimerEvent *event);

signals:
    void unitStartExe(tUnit eUnit);
    void flowEndTest();
    void keyClicked(QString key);
    void theUnitResult(bool res);


};

#endif // MODEL_TFLOW_H
