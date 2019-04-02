#ifndef MODEL_INCLUDE_H
#define MODEL_INCLUDE_H

#include <QObject>
#include <QDebug>
#include <QCoreApplication>
#include <QMessageBox>
#include <QDir>
#include <QTreeWidgetItem>
#include <QPainter>
#include <QProxyStyle>
#include <QDateTime>


#define cout        qDebug() << "[" <<QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss ddd")<< __FILE__ << ":"<< __FUNCTION__ << ":" << __LINE__ << "]"
#define toStr(num)  QString::number(num)
#define ON          true
#define OFF         false

//程序运行处理目录:
#define appDirPath QCoreApplication::applicationDirPath()

#define configPath(info)        appDirPath+"/config/"+info
#define ResultPath(itemName)    appDirPath+"/result/"+itemName
#define REPORTPath(info)        appDirPath+"/ATReport/"+info

//定义颜色：用于进度条显示
#define GRAY    QColor(199,199,199)   //灰
#define GREEN   QColor(0,209,0)      //绿
#define BLUE    QColor(0,85,255)      //蓝
#define RED     QColor(255,5,5)        //红
#define WHITE   QColor(255,255,255)  //白
#define BLACK   QColor(0,0,0)

#define DEVHID              "VID_18D1&PID_D002" //安卓设备硬件ID匹配

/*************************************************************
/ADB指令宏定义
*************************************************************/
#define PROSYS              0                   //系统进程号
#define ADBDevs                             "adb devices"       //扫描连接设备
#define ADBWIFICNT                          "adb connect "      //连接无线
#define ADBWIFIDISCNT                       "adb disconnect "   //端口无线
//获取设备信息
#define GETPROP                             ("adb shell getprop")
#define GETPROP_S(devNum)                   ("adb -s "+devNum+" shell getprop")
//获取界面信息
#define ACTIVITYFACE                        ("adb shell dumpsys activity|findstr  mF")
#define ACTIVITYFACE_S(devNum)              ("adb -s "+devNum+" shell dumpsys activity|findstr mFocusedActivity")
//捕捉图片//adb shell screencap -p /sdcard/a.png
#define SCREENCAP                  ("adb shell screencap -p /sdcard/screen.png")
#define SCREENCAP_S(devNum)         ("adb -s "+devNum+" shell screencap -p /sdcard/screen.png")
//导出图片
#define PULLFile(desPath)           ("adb pull /mnt/sdcard/screen.png "+desPath)
#define PULLFile_S(devNum,desPath)  ("adb -s "+devNum+" pull /mnt/sdcard/screen.png "+desPath)
//列举进程
#define PSGREP(appStr)                      ("adb shell ps | grep "+appStr)
#define PSGREP_S(devNum,appStr)             ("adb -s "+devNum+" shell ps | grep "+appStr)
//杀死进程
#define KILL_9(Str)                         ("adb shell kill -9 "+Str)
#define KILL_9_S(devNum,Str)                ("adb -s "+devNum+" shell kill -9 "+Str)
//修改文件名
#define RENAME(oldName,newName)             ("rename "+oldName+" "+newName)
//运行生成报告脚本
#define PYTHONREPORT(flpath)                ("python "+REPORTPath("run.py") +" "+flpath)



/*************************************************************
/串口：命令宏定义
*************************************************************/
#define CmdACKDelay 500     //命令响应延时
#define CmdReSendTimer 3   //定义命令重复发送次数
//CMD
#define CMDCANChannel      0x18    //打开/关闭CAN1
#define CMDItemRead         0x20
#define CMDItemWrite        0x21

//CMDKey1~36  0x22~0x46  any:因此该中间命令不可用
#define CMDDownloadKey      0x22 //定义串口传输命令:按键资源信息  Dat:isUse + type + CANId + CANDat1 + CANDat2
#define CMDSaveKeyInfo      0x23
#define CMDUploadKey      0x24

//CMD Control:0x50起
#define CMDClickedKey       0x50    //点击按键操作：Dat：KeyNum（1~36）+ON/OFF

//CMD param:hardware
#define Upload_SingleCurrent     0x30
#define Upload_CircularCurrent	0x31
#define CMDOverCurrentUp		0x32

#define Upload_SingleAudio       	0x33
#define Upload_CircularAudio     0x34
#define CMDOverAudioUp		0x35

#define Upload_SingleVB        		0x36
#define Upload_CircularVB       	0x37
#define CMDOverVBUp			0x38

//////////////////////////////////////////////
#define MaxKey              36
#define fixedKeyNum         3       //固定按键信息3个: ACC BAT CCD 信息固定后不可编辑

//定义CAN通道类型:
typedef enum
{
    CAN1CHANNEL,
    CAN2CHANNEL,
    CANSingle
}CANType_e;



extern QString WorkItem;
extern int WorkCurrent;            //工作电流
extern int WorkFrequency;          //测试次数
extern bool ReportCreat;           //创建报告


extern QString devNumber;
extern int Current;
extern int Volt;
extern bool SoundV;
//extern QStringList proList;
extern QStringList ShowList;
extern QString savePath;
extern QString AccKey;
extern QString BatKey;

extern QString testString;

extern bool actIsRunning;

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

/*************************************************************
/信息采集：
/高8位为保留位，低8位：低4位为1bit数据判断，高4位为2bit判断（第2位为用在动作前后的标志）
*************************************************************/
typedef enum
{
    COLCURRENT  = 0x0001,
    COLSOUND    = 0x0002,
    COLRES1     = 0x0004,
    COLRES2     = 0x0008,

    COLFACE         = 0x0010,   //采集界面信息
    COLFACESITE     = 0x0020,   //采集界面位置：动作执行前0或后1
    COLPICTURE      = 0x0040,   //采集图片
    COLPICTURESITE  =0x0080     //采集图片位置：动作执行前0或后1
}collect_type_e;


typedef enum
{
    ACT_NULL,
    ACT_KEY,
    ACT_SCRIPT
}ActType_e;

typedef enum
{
    NODealERROR,            //不进行错误处理
    OVERTIMEERR,        //超时处理

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

    //information 采集:
    uint16_t infoFlag; //详见:collect_type_e
    uint8_t errorDeal;  //错误处理机制：默认暂停 详见：error_type_e

    //time Deal:
    timeParam timeDeal;

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

/*************************************************************
/定义按键类型：
/Hard:硬件控制
/Can1_1:协议控制CAN1——单协议数据
/Can1_2:协议控制CAN1——双协议数据
*************************************************************/
typedef enum
{
    HardACC,
    HardBAT,
    HardCCD,
    HardLamp,
    HardBrake,
    HardRes,
    Can1_1,
    Can1_2,
    Can2_1,
    Can2_2
}kType;

/*************************************************************
/定义按键控制结构体：
/name:按键名
/tab:按键编号:同命令挂钩，可通过标号来执行按键
/des:描述按键
/type:按键类型
/CANID:CanID
/CANDat1:数据1
/CANDat2:数据2（双协议数据时使用）
*************************************************************/
typedef struct
{
    QString name;

    bool isUse;
    //uint8_t tab;
    QString des;

    kType type;

    QString CANID;
    QString CANDat1;
    QString CANDat2;

}keyControl;

/*************************************************************
/用户管理：
*************************************************************/
typedef enum
{
    OnlyUser,
    Administrator,
}Permissions_type_e;

typedef enum
{
    l_Chinese,
    l_English
}language_type_e;

typedef struct
{
    uint8_t Permissions;//权限
    QString userName;
    QString userPassword;
    uint8_t language;

}user_type_s;

extern user_type_s userLogin;

/*************************************************************
/类：重新画列表窗口显示
*************************************************************/
class CustomTabStyle : public QProxyStyle
{
public:
    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
        const QSize &size, const QWidget *widget) const
    {
        QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
        if (type == QStyle::CT_TabBarTab) {
            s.transpose();
            s.rwidth() = 90; // 设置每个tabBar中item的大小
            s.rheight() = 30;
        }
        return s;
    }

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
    {
        if (element == CE_TabBarTabLabel) {
            if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
                QRect allRect = tab->rect;

                if (tab->state & QStyle::State_Selected) {
                    painter->save();
                    painter->setPen(0x3399FF);
                    painter->setBrush(QBrush(0x3399FF));//深蓝   //0x89cfff--浅蓝
                    painter->drawRect(allRect.adjusted(6, 6, -6, -6));
                    painter->restore();
                }
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);
                if (tab->state & QStyle::State_Selected) {
                    painter->setPen(0xf8fcff);
                }
                else {
                    painter->setPen(0x0A0A0A);//接近黑色  //0x5d5d5d--深灰
                }

                painter->drawText(allRect, tab->text, option);
                return;
            }
        }

        if (element == CE_TabBarTab) {
            QProxyStyle::drawControl(element, option, painter, widget);
        }
    }
};



bool rangeJudgeTheParam(range_type_e rJudge,uint16_t param1,uint16_t param2,uint16_t value);

//QString getTestType(tType type);
QString getRangeJudge(range_type_e judge);
QString getSoundJudge(sound_type_e judge);
QString getCheckType(chk_type_e chk);
QString getCompareType(compare_type_e compare);


//keyControl *getkeyControlNull();
void getkeyControlNull(keyControl *keyInfo);
QString getKeyType(kType type);


bool appendTheResultToFile(QString SaveStr);
bool appendThePropertiesToFile(QString path,QString SaveStr);
void appendTheExecLogInfo(QString SaveStr);


QTreeWidgetItem *AddTreeNode(QTreeWidgetItem *parent, uint8_t flags,QStringList list);


void startAction(QString actStr);
void clearAction();

QString getDevNumber();

void initNullChkParam(checkParam *chkParam);
int getKeyNumber(QString key);


#endif // MODEL_INCLUDE_H
