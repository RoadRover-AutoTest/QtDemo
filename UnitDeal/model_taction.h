#ifndef MODEL_TACTION_H
#define MODEL_TACTION_H

#include <QObject>
#include <QTimerEvent>

#include "Models/model_delay.h"
#include "Models/model_process.h"

#include "Models/model_include.h"
#include "Models/model_string.h"
#include "UnitDeal/testunit.h"


/*定义测试动作类*/
class Model_tAction:public QObject
{
    Q_OBJECT
public:
    Model_tAction(int loop,tAction *Action);
    ~Model_tAction();

private:
    tAction *actionDeal;
    testUnit *unitDeal;

    typedef enum
    {
        start,
        collectInfo,
        exeAction,
        chkAction,
        wait,
        waitover,
        pauseState,
        actover
    }time_deal_e;

    time_deal_e timeState;
    time_deal_e nextState;
    time_deal_e lastState;
    uint64_t timeCount;

    bool testResult;
    uint64_t reChkCount;     //重复监测计数
    uint64_t overtimeAct;
    uint64_t TimeDelay1S;
    uint8_t soundTimer;//
    uint16_t colInfoFlag;       // sizeColInfo_e 用来标记已完成的信息采集
    uint16_t actInfoFlag;
    bool colSize;
    int timeID_T;

    int iniLoop;

    bool IsFirstMemory;

    void theActionInitParam();
    void theActionStartDeal();
    void theActionCollectInfoDeal(bool curSize,uint16_t infoFlag);
    void theActionExecuateDeal();
    void theActionChangedDeal(QList <changedParam>testChanged);
    void theActionCheckReault(QList <checkParam> testChk);
    void theActionPauseDeal();

    Model_Process *PRODeal;
    bool isPRORunning;
    int timerProID;
    QString currentCMDString;
    QStringList proList;
    QStringList deviceList;
    bool IsOKCMDRunned;         //  命令运行完成标志，用来标记取字符串OK
    uint8_t IsReRunning;//重新运行次数
    QString proItemString;
    QString proCMDString;


    void initProcessDeal();
    void deleteProcessDeal();
    void proStopSysLogcat();
    void proStopSysUiautomator();
    bool proSysIsRunning();
    void timerProIDDeal();
    void onProcessEXECmd(QString cmdType);

private slots:
    void onProcessOutputSlot(int pNum,QString String);
    void onProcessOverSlot(uint8_t pNum);

protected:
    void timerEvent(QTimerEvent *event);

signals:
    void theActionOverTest(bool result);


};

#endif // MODEL_TACTION_H
