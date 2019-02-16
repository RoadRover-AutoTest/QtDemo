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

#define cout        qDebug() << "[" << __FILE__ << ":"<< __FUNCTION__ << ":" << __LINE__ << "]"
#define toStr(num)  QString::number(num)
#define ON          true
#define OFF         false
#define KeyINFOFile QCoreApplication::applicationDirPath()+"/config/KeyInfo.xml"
#define SYSInfo     QCoreApplication::applicationDirPath()+"/config/config.ini"
#define ResultPath  QCoreApplication::applicationDirPath()+"/result"
#define REPORTPath  QCoreApplication::applicationDirPath()+"/ATReport"

//定义颜色：用于进度条显示
#define GRAY    QColor(199,199,199)   //灰
#define GREEN   QColor(0,209,0)      //绿
#define BLUE    QColor(0,85,255)      //蓝
#define RED     QColor(255,5,5)        //红
#define WHITE   QColor(255,255,255)  //白
#define BLACK   QColor(0,0,0)

//测试流宏定义
#define MAXFLOW                 0xFF            //最大测试流 //且0xFF为自定义时当前流程使用,实际可用0xFE
#define setSingleCount          2               //"测试次数"50次
#define setAllCount             2               //设置整体循环次数
#define SCRIPTDealTime          0               //定义脚本处理时间为0

#define DesACC_Script "Acc环境下的脚本测试：ACC关--ACC开--运行脚本"




/*************************************************************
/ADB指令宏定义
*************************************************************/
#define DEVHID              "VID_18D1&PID_D002" //安卓设备硬件ID匹配

#define PROSYS              0                   //系统进程号
#define PROREPORT           1                   //生成报告、避免与主函数进程相同影响判断
#define ADBDevs                 "adb devices"       //扫描连接设备
#define ADBWIFICNT              "adb connect "      //连接无线
#define ADBWIFIDISCNT           "adb disconnect "   //端口无线
#define ADBGetprop              "adb shell getprop" //获取设备信息

#define SHELLFACE               " shell dumpsys activity|findstr  mF"   //获取界面信息
#define SHELLPROP               " shell getprop"                        //获取设备信息

#define PYTHONREPORT            "python "+REPORTPath+"/run.py "












/*************************************************************
/串口：命令宏定义
*************************************************************/
#define CmdACKDelay 500     //命令响应延时
#define CmdReSendTimer 2   //定义命令重复发送次数
//CMD
#define CMDCAN1Channel      0x18    //打开/关闭CAN1
#define CMDCAN2Channel      0x19    //打开/关闭CAN2
#define CMDItemRead         0x20
#define CMDItemWrite        0x21

//CMDKey1~36  0x22~0x46  any:因此该中间命令不可用
#define CMDKey1             0x22 //定义串口传输命令:按键资源信息  Dat:isUse + type + CANId + CANDat1 + CANDat2
#define MaxKey              36
#define fixedKeyNum         3       //固定按键信息3个: ACC BAT CCD 信息固定后不可编辑

//CMD Control:0x50起
#define CMDClickedKey       0x50    //点击按键操作：Dat：KeyNum（1~36）+ON/OFF

//CMD param:hardware
#define CMDWorkCurrent      0x60
#define CMDVoltParam        0x61
#define CMDSoundCheck       0x62


extern QString WorkItem;
extern int WorkCurrent;            //工作电流
extern int WorkFrequency;          //测试次数
extern bool ReportCreat;           //创建报告



extern int Current;
extern int Volt;
extern QStringList proList;
extern QStringList ShowList;
extern QString savePath;
extern QString AccKey;
extern QString BatKey;

extern QString testString;
extern QString offFace;
extern QString onFace;

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
/定义声音判断：
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
    CHKMEMORY,
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
/endHAVESound:末尾值有声音
/endNOSound:末尾值无声音
/HCountthanNCount:有声音次数大于等于无声音次数
/HCountlessNCount:有声音次数小于等于无声音次数
/noHSoundCount:不存在有声音值
/noNSoundCount:不存在无声音值
*************************************************************/
typedef enum
{
    endHAVESound,
    endNOSound,
    HCountthanNCount,
    HCountlessNCount,
    noHSoundCount,
    noNSoundCount
}sound_type_e;

/*************************************************************
/检测测试动作：
/chk_type_e：测试类型
/range_type_e：测试范围
/sound_type_e：声音检测
/logContains:log检测
/hReault：硬件检测结果判断
/any:图像的变量：存储第一次获取的图像，之后的与其比较
*************************************************************/
typedef struct
{
    chk_type_e check;

    range_type_e range;
    int min;
    int max;

    sound_type_e sound;

    QString logContains;

    bool isMemory;          //true:回到记忆界面  false：读取到界面不进行判断；
    bool isCompareFirstPic; //true:比较首次采集图片  false:非首次图片比较，即与测试单元中其他动作下采集图片比较

    bool hReault;
}checkParam;

/*************************************************************
/时间处理：
/测试等待处理时间
/检测等待时间
/最长等待时间
/备注：若检测时间《等待时间   达到检测时间时检测，之后继续等待:因此check时间商用在等待结束前检测的处理
    若检测时间》等待时间  达到等待时间开始检测，直到检测完成，相当于按等待时间来检测 若无检测直接下一项测试
    若时间超过最长时间    结束检测及等待，判断为失败
*************************************************************/
typedef struct
{
    uint64_t wait;
    uint64_t check;
    uint64_t end;
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

    COLFACE     = 0x0010,
    COLPICTURE  = 0x0040
}collect_type_e;

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

    //information 采集:
    uint16_t infoFlag; //详见:collect_type_e

    //time Deal:
    timeParam timeDeal;

    //chk Deal:
    QList <checkParam>checkDeal;

    //changed Deal:
    QList <changedParam>changedDeal;

    //result Deal:重大事件结束测试/继续测试等any  规划

}tAction;

/*************************************************************
/定义测试单元：
/tNum:测试单元编号  未加入测试流的测试流编号默认为MAXFLOW
/actTest:测试动作集合:执行时按顺序执行
/cycleCount:测试循环次数
*************************************************************/
typedef struct
{
    //int tNum;
    QString name;

    QList <tAction> actTest;

    int cycleCount;

    QString unitDes;//测试描述


}tUnit;



typedef struct
{
    QString onParam;
    QString onTime;

    QString offParam;
    QString offTime;
}param;

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
    HardRes,
    Can1_1,
    Can1_2
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


keyControl *getkeyControlNull();
QString getKeyType(kType type);


bool appendTheResultToFile(QString SaveStr);
bool appendThePropertiesToFile(QString SaveStr);


QTreeWidgetItem *AddTreeNode(QTreeWidgetItem *parent, uint8_t flags,QStringList list);

#endif // MODEL_INCLUDE_H
