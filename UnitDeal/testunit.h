#ifndef TESTUNIT_H
#define TESTUNIT_H

#include <QObject>
#include "Models/model_include.h"
#include "Models/model_piccompare.h"
typedef enum
{
    WaitTime,
    BatVolt
}changed_type_e;


/*************************************************************
/检测参数变动：
/dir:变动方向
/min：变动最小值
/max：变动最大值
/step：变动幅度
/备注：使用Map来定义变动类型，此为键与值的关系，值为字符串类型，可用于显示当前处理的类型，省去再转换的过程
*************************************************************/
typedef struct
{
    changed_type_e changed;

    bool dir;

    uint64_t min;
    uint64_t max;

    uint64_t step;
}changedParam;



/*************************************************************
/定义检测类型：
/CHKNothing:无检测
/CHKCurrent:检测电流:范围检测
/CHKVlot:检测电压：范围检测
/CHKSound:检测声音
/CHKScript:检测脚本
/CHKMEMORY:检测记忆（关机重启记忆）：无法确定关机前相比较的数据，因此处理的数据在测试过程中处理
/CHKRES:检测保留
*************************************************************/
typedef enum
{
    CHKCurrent,
    CHKVlot,
    CHKSound,
    CHKScript,
    CHKInterface,
    CHKADBPIC,
    CHKRES
}chk_type_e;



/*************************************************************
/定义范围检测
*************************************************************/
typedef enum
{
    GE,//>=
    GT,//>
    LE,//<=
    LT,//<
    EQ,//==
    NE,// !=
    GELE//>= && <=
}range_type_e;

/*************************************************************
/定义声音判断：
/HaveSound:存在有声音值
/NOSound:存在无声音值
/noHSoundCount:不存在有声音值
/noNSoundCount:不存在无声音值
/HCountthanNCount:有声音次数大于等于无声音次数
/HCountlessNCount:有声音次数小于等于无声音次数
*************************************************************/
typedef enum
{
    HaveSound,
    NOSound,
    noHSoundCount,
    noNSoundCount,
    HCountthanNCount,
    HCountlessNCount
}sound_type_e;

/*************************************************************
/定义比较判断：
/0x00:不比较（若是存在记忆检测将只判断是否获取到界面）
/0x01:记忆检测：和上次动作获取信息比较
/0x02:自身比较：每次均和首次采集信息比较
/0x03:本地比较：和本地文件比较（通常为图片）
*************************************************************/
typedef enum
{
    NoCompare,
    MemoryCompare,
    SelfCompare,
    LocalCompare
}compare_type_e;

/*************************************************************
/检测测试动作：
/any:图像的变量：存储第一次获取的图像，之后的与其比较
*************************************************************/
typedef struct
{
    chk_type_e check;       //决定检测参数//chk_type_e：测试类型

    range_type_e range;//range_type_e：测试范围
    int min;
    int max;

    sound_type_e sound;//sound_type_e：声音检测

    QString logContains;//logContains:log检测

    compare_type_e infoCompare; //判断比较方式
    QString comTarget;//比较对象

    bool hReault;//hReault：硬件检测结果判断
}checkParam;

/*************************************************************
/时间处理：
/备注：若检测时间《等待时间   达到检测时间时检测，之后继续等待:因此check时间商用在等待结束前检测的处理
    若检测时间》等待时间  达到等待时间开始检测，直到检测完成，相当于按等待时间来检测 若无检测直接下一项测试
    若时间超过最长时间    结束检测及等待，判断为失败
*************************************************************/
typedef struct
{
    uint64_t wait;//测试等待处理时间
    uint64_t check;//检测等待时间
    uint64_t end;//最长等待时间
}timeParam;

typedef enum
{
    ACT_NULL,
    ACT_KEY,
    ACT_SCRIPT,
    ACT_BATVolt
}ActType_e;

typedef enum
{
    NODealERROR,            //不进行错误处理
    OVERTIMEERR,        //超时处理
    CHKERROR,

}error_type_e;


/*************************************************************
/定义测试单元：（尽可能包含测试过程中所有的处理参数）
/动作处理--时间处理--检测处理--变动处理
/备注：休眠唤醒测试与BAT测试方案：
/1.此类型定义变量，单独处理该2种测试；--不单独处理，仍定义测试单元处理
/2.测试类型处添加这2种测试，执行动作时下发指令，小板循环测试，时间达到，发送获取测试结果的指令
/采用第2种测试，既避免了在完整的测试流中添加特殊处理（特殊处理较多影响整个的测试流程分析），
/又可将bat电压改动放在小板中处理（避免了来回传递电压的处理）。
/actStr:动作字符串：为方控时：为协议数据，为脚本时：为执行命令
/备注：添加ACC休眠唤醒测试和BAT开关机测试
bat:电压值改变 KEY1:Bat:12
*************************************************************/
typedef struct
{
    QString actName;

    //action Deal:
    QString actStr;
    uint8_t actFlag;//动作类型：0-空 1-KEY 2-Script

    uint8_t errorDeal;  //错误处理机制：默认暂停 详见：error_type_e

    //time Deal:
    timeParam timeDeal;

    //采集信息处理
    QStringList colInfoList;//ACT1:PIC:BACK  组成结构：动作序号+类型+执行位置

    //chk Deal:
    QList <checkParam>checkDeal;

    //changed Deal:
    QList <changedParam>changedDeal;
}tAction;


/*************************************************************
/定义测试单元：
/tNum:测试单元编号  未加入测试流的测试流编号默认为MAXFLOW
/actTest:测试动作集合:执行时按顺序执行
/cycleCount:测试循环次数
*************************************************************/
typedef struct
{
    QString name;
    QList <tAction> actTest;
    int cycleCount;
    QString unitDes;//测试描述
}tUnit;

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

/*
 * 标记采集的信息标志：
*/
typedef enum
{
    SIZE_Current = 0x0001,
    SIZE_Volt = 0x0002,
    SIZE_Sound = 0x0004,
    SIZE_Interface = 0x0008,
    SIZE_Picture = 0x0010
}sizeColInfo_e;//标记信息采集


#define ACT_Front   false
#define ACT_Back    true

//*****************************************************************
//全局函数
//QString getTestType(tType type);
QString getRangeJudge(range_type_e judge);
QString getSoundJudge(sound_type_e judge);
QString getCheckType(chk_type_e chk);
QString getCompareType(compare_type_e compare);

void initNullChkParam(checkParam *chkParam);

bool rangeJudgeTheParam(range_type_e rJudge,uint16_t param1,uint16_t param2,uint16_t value);


class testUnit:public QObject
{
    Q_OBJECT
public:
    testUnit();
    ~testUnit();

    bool chkCurrent(int value,checkParam range);
    bool chkVolt(int value,checkParam range);
    bool chkSound(QList <bool>SoundList,checkParam sound);
    bool chkScript(QString filePath,checkParam script);
    bool chkInterface(QString infoStr,checkParam memory);
    bool chkADBPic(QString infoStr, checkParam adbpic);
    bool chkRes(checkParam res);

    QString ActColInfo_Read(bool size,QString info,QStringList colInfoList);

    void ColInfo_Append(uint16_t infoflag,storageInfo_type_s infoDat,QList <checkParam> checkDeal);
    QString ColInfo_find(QString name,QList <storageInfo_type_s> infoDat);


    void inittActionParam(tAction *tact);
    void actAppend_ACCON(int actNum,tAction *kAction,QList <keyControl> keyList);
    void actAppend_ACCOFF(int actNum,tAction *kAction,QList <keyControl> keyList);
    void actAppend_BATON(int actNum,tAction *kAction,QList <keyControl> keyList);
    void actAppend_BATOFF(int actNum,tAction *kAction,QList <keyControl> keyList);
    void actAppend_CCDON(int actNum,tAction *kAction,QList <keyControl> keyList);
    void actAppend_CCDOFF(int actNum,tAction *kAction,QList <keyControl> keyList);
    void actAppend_key(QString keyName,tAction *kAction,QList <keyControl> keyList);
    void actAppend_script(tAction *sAction);
    void actAppend_batVolt(tAction *bAction);

};

#endif // TESTUNIT_H
