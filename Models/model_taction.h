#ifndef MODEL_TACTION_H
#define MODEL_TACTION_H

#include <QObject>
#include <QTimerEvent>

#include "model_include.h"
#include "model_delay.h"

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
        waitnull,
        wait,
        waitover,
        checkover,
        endover
    }time_deal_e;

    time_deal_e timeState;
    uint64_t timeCount;

    bool testResult;
    //int passCount;      //测试通过次数，连续监测3次均通过判断为测试成功
    uint64_t reChkCount;     //重复监测计数
    uint64_t overtimeAct;


    int timeID_T;

    int iniLoop;

    bool IsFirstMemory;

    bool IsDealCheck();
    void evaluateTheAction(QString actStr);
    void theActionChangedDeal(QList <changedParam>testChanged);
    bool theActionResultDeal(QList <checkParam> testChk);
    void theActionCheckReault(QList <checkParam> testChk);

    bool chkCurrent(checkParam range);
    bool chkVolt(checkParam range);
    bool chkSound(checkParam sound);
    bool chkScript(checkParam script);
    bool chkMemory(checkParam memory);
    bool chkRes(checkParam res);

protected:
    void timerEvent(QTimerEvent *event);

signals:
    void theActionOverTest(bool result);


};

#endif // MODEL_TACTION_H
