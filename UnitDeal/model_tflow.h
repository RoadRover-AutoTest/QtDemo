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

private:
    Model_tUnit *unitDeal;
    QList <tUnit> *theFlow;    //测试流
    tUnit curUnit;

    int timeID_UF;      //测试单元及测试流执行定时器
    int reTime;//最长1分钟
    int unitIndex;
    int cycleCount;
    bool isUnitDeal;


    typedef enum
    {
        start,
        dealunit,
        waitnull,
        waitrecover,
        unitover,
        over,
        endflow
    }flow_type_e;

    flow_type_e flowState;
    flow_type_e lastState;




    bool chkWorkState();

private slots:
    void onStartTheUnitSlot();
    void onEndTheUnitSlot();
    void onUnitResultSlot(bool result);

protected:
    void timerEvent(QTimerEvent *event);

signals:
    void unitStartExe(tUnit eUnit);
    void flowEndTest();
    void keyClicked(QString key);
    void theUnitResult(bool res);


};

#endif // MODEL_TFLOW_H
