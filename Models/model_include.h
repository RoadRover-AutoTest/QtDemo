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


#define ColSOUNDTimer 11    //采集声音次数

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
#define ACTIVITYFACE                        ("adb shell dumpsys activity|findstr  mFocusedActivity")
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
#define CmdACKDelay 1000     //命令响应延时
#define CmdReSendTimer 3   //定义命令重复发送次数
//CMD
#define CMD_LINK_STATUS    0x01     //小屏连接状态
#define CMD_TIME_SYN		0x02	//时钟同步
#define CMD_MEDIA_INFO2    0x03     //媒体2
#define CMD_MEDIA_INFO     0x0A     //媒体


#define CMDCANChannel      0x18    //打开/关闭CAN1
#define CMDUploadCAN        0x19
#define CMDItemRead         0x20    //项目信息
#define CMDItemWrite        0x21
#define CMDDownloadKey      0x22 //定义串口传输命令:按键资源信息  Dat:isUse + type + CANId + CANDat1 + CANDat2
#define CMDSaveKeyInfo      0x23
#define CMDUploadKey        0x24
#define CMDBATPower         0x25       //BAT 电压
#define CMDUploadBatVal     0x26
#define CMDCCDPower         0x27        //CCD电压
#define CMDUploadCCDVal     0x28
#define CMDBATMaxVal        0x29
#define CMDSpeedVal        	0x2A		//调试速度

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

//temp:因CCDpower不能和预留IO做速度的接口同时操作，因此做一个切换
#define CMDTempReverse			0xFA


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
extern bool PauseState;






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
    HardCCDPower,
    Can1_1,
    Can1_2,
    Can2_1,
    Can2_2,
    KEYOTHER
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
    Designer,
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
void setDevNumberCount(int count);
bool NumberListIsSingle();


int getKeyNumber(QString key);


#endif // MODEL_INCLUDE_H
