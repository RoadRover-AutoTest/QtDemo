#include "Models/model_include.h"

//ATtoolConfig配置参数
QString WorkItem;           //当前测试项目
int WorkCurrent;            //工作电流
int WorkFrequency;          //测试次数
bool ReportCreat;           //创建报告




int Current=0;                  //当前测试电流
int Volt = 0;
QStringList proList;            //进程处理解析字符串列表：用来显示
QStringList ShowList;           //字符串列表：用来显示
QString savePath;                //测试序列保存为xml文件；测试结果记录保存为TXT文件，路径为序列文件所在路径，文件名为序列文件名

//特殊字符串标记，以便直接处理
QString AccKey;
QString BatKey;

QString testString;            //以此字符串传递action的执行字符串在主函数中运行
QString offFace;           //关机时处于的界面
QString onFace;

bool actIsRunning=false;

/*************************************************************
/函数功能：参数范围判断
/函数参数：rJudge:判断条件  param1：判断参数1  param2：判断参数2  value:需判断的数值
/函数返回：判断结果
*************************************************************/
bool rangeJudgeTheParam(range_type_e rJudge,uint16_t param1,uint16_t param2,uint16_t value)
{
    bool chkResult=false;

    switch(rJudge)
    {
    case GE:
        if(value>=param1)
            chkResult=true;
        break;
    case GT:
        if(value>param1)
            chkResult=true;
        break;
    case LE:
        if(value<=param1)
            chkResult=true;
        break;
    case LT:
        if(value<param1)
            chkResult=true;
        break;
    case EQ:
        if(value==param1)
            chkResult=true;
        break;
    case NE:
        if(value!=param1)
            chkResult=true;
        break;
    case GELE:
        if((value>=param1)&&(value<=param2))
            chkResult=true;
        break;
    }
    return chkResult;
}

/*************************************************************
/函数功能：获得空按键资源信息：主要对数据信息进行赋值，字符串默认为空
/函数参数：无
/函数返回：按键信息：空
*************************************************************/
keyControl *getkeyControlNull()
{
    keyControl *keyInfo=new keyControl;

    keyInfo->isUse=false;
    keyInfo->type=HardACC;

    keyInfo->des="No Information!";

    return keyInfo;
}

QString getKeyType(kType type)
{
    QString str;
    switch(type)
    {
    case HardACC:str="硬件ACC";break;
    case HardBAT:str="硬件BAT";break;
    case HardCCD:str="硬件CCD";break;
    case HardLamp:str="硬件LAMP";break;
    case HardRes:str="硬件保留";break;
    case Can1_1:str="单协议";break;
    case Can1_2:str="双协议";break;
    }
    return str;
}

/*************************************************************
/函数功能：根据枚举类型获得字符串显示
/函数参数：枚举类型
/函数返回：字符串
***********************************************************
QString getTestType(tType type)
{
    QString str;
    switch(type)
    {
    case KEY:str="方控";break;
    case SCRIPT:str="脚本";break;
    }
    return str;
}
**/

/*************************************************************
/函数功能：根据枚举类型获得字符串显示
/函数参数：枚举类型
/函数返回：字符串
*************************************************************/
QString getRangeJudge(range_type_e judge)
{
    QString str;
    switch(judge)
    {
    case GE:str=">=";break;//>=
    case GT:str=">";break;//>
    case LE:str="<=";break;//<=
    case LT:str="<";break;//<
    case EQ:str="==";break;//==
    case NE:str="!=";break;// !=
    case GELE:str=">= && <=";break;//>= && <=
    }
    return str;
}

/*************************************************************
/函数功能：根据枚举类型获得字符串显示
/函数参数：枚举类型
/函数返回：字符串
*************************************************************/
QString getSoundJudge(sound_type_e judge)
{
    QString str;
    switch(judge)
    {
    case endHAVESound:str="末尾值有声音";break;
    case endNOSound:str="末尾值无声音";break;
    case HCountthanNCount:str="有声音次数大于等于无声音次数";break;
    case HCountlessNCount:str="有声音次数小于等于无声音次数";break;
    case noHSoundCount:str="不存在有声音值";break;
    case noNSoundCount:str="不存在无声音值";break;
    }
    return str;
}

/*************************************************************
/函数功能：保存文件
/函数参数：SaveStr :保存字符串
/函数返回：无Text
/Append：在现有文件上添加   Text：文档从头编辑
*************************************************************/
bool appendTheResultToFile(QString SaveStr)
{
    QDir dir(savePath);
    if(!dir.exists())
    {
        if(dir.mkpath(savePath) == false) //创建多级目录
            return false;
    }

    QFile file(savePath+"/report.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        cout << "Cannot open file " << savePath+"/report.txt";
        return false;
    }
    QTextStream out(&file);
    out << SaveStr +"\r\n";

    file.close();
    return true;
}

/*************************************************************
/函数功能：保存文件:机器信息
/函数参数：SaveStr :保存字符串
/函数返回：无Text
/Append：在现有文件上添加   Text：文档从头编辑
*************************************************************/
bool appendThePropertiesToFile(QString SaveStr)
{
    QStringList pathList = savePath.split("/");

    QString path ;
    for(int i=0;i<pathList.length()-3;i++)
    {
        path += pathList.at(i) + "/";
    }

    QDir dir(path);
    if(!dir.exists())
    {
        if(dir.mkpath(path) == false) //创建多级目录
            return false;
    }

    QFile file(path+"properties.txt");
    bool isOK;

    if(SaveStr == "clear")
        isOK=file.open(QIODevice::WriteOnly | QIODevice::Text);
    else
        isOK=file.open(QIODevice::WriteOnly | QIODevice::Append);

    if (!isOK)
    {
        cout << "Cannot open file " << path+"properties.txt";
        return false;
    }

    QTextStream out(&file);
    if(SaveStr == "clear")
        out << "";
    else
        out << SaveStr;

    file.close();
    return true;
}

/*************************************************************
/函数功能：在树上添加节点
/函数参数：  父节点  属性  list填充数据
/函数返回：添加的节点
//flags:0x01:可编辑  0x02:字体颜色为红色
*************************************************************/
QTreeWidgetItem *AddTreeNode(QTreeWidgetItem *parent, uint8_t flags,QStringList list)
{
    QTreeWidgetItem * item=new QTreeWidgetItem(list);

    if(flags & 0x01)
    {
        item->setFlags(item->flags()|Qt::ItemIsEditable);
    }

    if(flags & 0x02)
    {
        for(int i=0;i<list.length();i++)
            item->setTextColor(i,RED);
    }

    parent->addChild(item);
    return item;
}






