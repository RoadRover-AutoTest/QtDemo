#ifndef MODEL_TUNIT_H
#define MODEL_TUNIT_H

#include <QObject>

#include "model_include.h"
#include "model_taction.h"


class Model_tUnit:public QObject
{
    Q_OBJECT
public:
    Model_tUnit(tUnit *testUnit);
    ~Model_tUnit();

    int getTheUnitLoop();

private:
    tUnit UnitDeal;                //测试单元:不使用指针，action处理变动后仅在内循环修改，使用指针将会修改全局数据
    Model_tAction *ActDeal;         //测试动作处理
    tAction curAction;

    int actIndex;                   //当前测试动作指针，代表测试项
    int cycleCount;                 //测试计数
    QVector<bool> arrayResult;      //存储测试单元执行结果,动作若无检测不添加，最终判断的为有检测的动作，此为数组结果
    bool isActionDeal;

    typedef enum
    {
        start,
        initUnit,
        wait,
        dealAction,
        overAction,
        overUnit,
        over
    }test_deal_e;

    test_deal_e testState;

    int timeID_U;

private slots:
    void ontheActionResultSlot(bool result);

protected:
    void timerEvent(QTimerEvent *event);

signals:
    void theUnitStart();
    void theUnitEnd();
    void theUnitResult(bool res);
};

#endif // MODEL_TUNIT_H
