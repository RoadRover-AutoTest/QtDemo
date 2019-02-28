#ifndef MODEL_TACTION_H
#define MODEL_TACTION_H

#include <QObject>
#include <QTimerEvent>

#include "model_delay.h"
#include "model_process.h"
//#include "model_piccompare.h"
#include "model_include.h"

#define ACT_Front   false
#define ACT_Back    true

/*存储采集数据类型：
 * 数据名：定义和动作或数据类型相关，以便查找
 * 信息：未固定数据类型，可为多种数据类型
*/
typedef struct
{
    QString name;
    QVariant information;
}storageInfo_type_s;

extern QList <storageInfo_type_s> fixedInfo;

extern QList <storageInfo_type_s> tempFaceInfo;
extern QList <storageInfo_type_s> tempPicInfo;

extern QList <bool> tempSoundInfo;


/*定义测试动作类*/
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
    uint64_t reChkCount;     //重复监测计数
    uint64_t overtimeAct;
    uint64_t collectTimeDelay;

    uint16_t colInfoFlag;       //用来标记已完成的信息采集
    uint16_t infoFlag;
    int timeID_T;

    int iniLoop;

    bool IsFirstMemory;

    bool judgeIsCollectInfo(bool site);
    void collectInfoDeal(uint16_t infoFlag);
    void evaluateTheAction(QString actStr);
    void theActionChangedDeal(QList <changedParam>testChanged);
    void theActionCheckReault(QList <checkParam> testChk);

    bool chkCurrent(checkParam range);
    bool chkVolt(checkParam range);
    bool chkSound(checkParam sound);
    bool chkScript(checkParam script);
    bool chkInterface(checkParam memory);
    bool chkADBPic(checkParam adbpic);
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
        CMD_FACE,
        CMD_ADBPic
    }cmd_type_e;
    cmd_type_e proCMD;
    QString proCMDString;


    void initProcessDeal();
    void deleteProcessDeal();
    void proStopSysLogcat();
    void proStopSysUiautomator();
    bool proSysIsRunning();
    void timerProIDDeal();
    void onProcessEXECmd(cmd_type_e cmdType);
    void infoAppendDeal(uint16_t infoflag,storageInfo_type_s infoDat);


private slots:
    void onProcessOutputSlot(int pNum,QString String);
    void onProcessOverSlot(uint8_t pNum);

protected:
    void timerEvent(QTimerEvent *event);

signals:
    void theActionOverTest(bool result);


};

#endif // MODEL_TACTION_H
