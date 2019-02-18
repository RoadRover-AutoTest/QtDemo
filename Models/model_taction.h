#ifndef MODEL_TACTION_H
#define MODEL_TACTION_H

#include <QObject>
#include <QTimerEvent>

#include "model_include.h"
#include "model_delay.h"
#include "Models/model_process.h"

#define ACT_Front   false
#define ACT_Back    true


class Model_tAction:public QObject
{
    Q_OBJECT
public:
    Model_tAction(int loop,tAction *Action);
    ~Model_tAction();

private:
    tAction *actionDeal;

    typedef enum
    {
        start,
        collectInfo,
        exeAction,
        chkAction,
        wait,
        waitover,
        actover

    }time_deal_e;

    time_deal_e timeState;
    time_deal_e nextState;
    uint64_t timeCount;

    bool testResult;
    //int passCount;      //测试通过次数，连续监测3次均通过判断为测试成功
    uint64_t reChkCount;     //重复监测计数
    uint64_t overtimeAct;

    uint16_t colInfoFlag;       //用来标记已完成的信息采集
    int timeID_T;

    int iniLoop;

    bool IsFirstMemory;

    bool judgeIsCollectInfo(bool site);
    void collectInfoDeal();
    void evaluateTheAction(QString actStr);
    void theActionChangedDeal(QList <changedParam>testChanged);
    void theActionCheckReault(QList <checkParam> testChk);

    bool chkCurrent(checkParam range);
    bool chkVolt(checkParam range);
    bool chkSound(checkParam sound);
    bool chkScript(checkParam script);
    bool chkMemory(checkParam memory);
    bool chkRes(checkParam res);



    Model_Process *PRODeal;
    bool isPRORunning;
    int timerProID;
    QString currentCMDString;
    QStringList proList;
    QStringList deviceList;

    typedef enum
    {
        CMD_NULL,
        CMD_script,
        CMD_ListDev,
        CMD_FACE
    }cmd_type_e;
    cmd_type_e proCMD;


    void initProcessDeal();
    void deleteProcessDeal();
    void proStopSysLogcat();
    void proStopSysUiautomator();
    bool proSysIsRunning();
    void timerProIDDeal();
    void onProcessEXECmd(cmd_type_e cmdType);

private slots:
    void onProcessOutputSlot(int pNum,QString String);
    void onProcessOverSlot(uint8_t pNum);

protected:
    void timerEvent(QTimerEvent *event);

signals:
    void theActionOverTest(bool result);


};

#endif // MODEL_TACTION_H
