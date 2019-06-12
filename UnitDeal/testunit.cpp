#include "testunit.h"

QList <storageInfo_type_s> fixedInfo;       //固定界面信息：随测试序列刷新：即每次开始测试启动时清空，测试过程中首次测试某一项，添加到列表中，之后的测试单元测试时将于其相对应数据比较

QList <storageInfo_type_s> tempFaceInfo;        //临时界面信息：随测试单元刷新：即测试单元开始可填写数据，结束即清空
QList <storageInfo_type_s> tempPicInfo;         //临时图片信息


testUnit::testUnit()
{

}


testUnit::~testUnit()
{

}


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
QString getSoundJudge(sound_type_e judge)
{
    QString str;
    switch(judge)
    {
    case HaveSound:str="存在有声音值";break;
    case NOSound:str="存在无声音值";break;
    case noHSoundCount:str="声音值全为无";break;
    case noNSoundCount:str="声音值全为有";break;
    case HCountthanNCount:str="有声音次数大于等于无声音次数";break;
    case HCountlessNCount:str="有声音次数小于等于无声音次数";break;

    }
    return str;
}
QString getCheckType(chk_type_e chk)
{
    QString str;
    switch(chk)
    {
        case CHKCurrent:str = "检测电流";break;
        case CHKVlot:str = "检测电压";break;
        case CHKSound:str = "检测声音";break;
        case CHKScript:str = "检测脚本LOG";break;
        case CHKInterface:str = "检测界面";break;
        case CHKADBPIC:str = "检测图片";break;
        case CHKRES:str = "检测保留";break;
    }
    return str;
}
QString getCompareType(compare_type_e compare)
{
    QString str;
    switch(compare)
    {
        case NoCompare:str = "不比较";break;
        case MemoryCompare:str = "记忆检测";break;
        case SelfCompare:str = "自身校验";break;
        case LocalCompare:str = "本地校验";break;
    }
    return str;
}

/*************************************************************
/函数功能：初始化检测参数
/函数参数：无
/函数返回：wu
*************************************************************/
void initNullChkParam(checkParam *chkParam)
{
    chkParam->check =CHKRES;
    chkParam->range=GE;
    chkParam->max=0;
    chkParam->min=0;
    chkParam->hReault=0;
    chkParam->sound=HaveSound;
    chkParam->infoCompare=NoCompare;
    chkParam->logContains.clear();
}

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
/函数功能：检测电流
/函数参数：范围检测参数
/函数返回：wu
*************************************************************/
bool testUnit::chkCurrent(int value,checkParam range)
{
    //int value = colInfoDat.Current;
    ShowList << tr("checkTheAction:检测电流...")+toStr(value);
    bool result = rangeJudgeTheParam(range.range,range.min,range.max,value);

    if(range.range != GELE)
        appendTheResultToFile("Judge:Current:"+getRangeJudge(range.range)+toStr(range.min));
    else
        appendTheResultToFile("Judge:Current:>="+toStr(range.min)+"  <="+toStr(range.max));
    appendTheResultToFile("Check:Current:"+toStr(value));
    appendTheResultToFile("Result:Current:"+toStr(result));
    return result;
}

/*************************************************************
/函数功能：检测电压
/函数参数：范围检测参数
/函数返回：wu
*************************************************************/
bool testUnit::chkVolt(int value,checkParam range)
{
    //int value = colInfoDat.volt;
    ShowList << tr("checkTheAction:检测电压...")+toStr(value);
    bool result = rangeJudgeTheParam(range.range,range.min,range.max,value);

    if(range.range != GELE)
        appendTheResultToFile("Judge:Volt:"+getRangeJudge(range.range)+toStr(range.min));
    else
        appendTheResultToFile("Judge:Vlot:>="+toStr(range.min)+"  <="+toStr(range.max));
    appendTheResultToFile("Check:Volt:"+toStr(value));
    appendTheResultToFile("Result:Volt:"+toStr(result));
    return result;
}

/*************************************************************
/函数功能：检测声音
/函数参数：声音检测参数
/函数返回：wu
*************************************************************/
bool testUnit::chkSound(QList <bool>SoundList,checkParam sound)
{
    ShowList<< tr("checkTheAction:检测声音...");
    bool result=false;
    switch(sound.sound)
    {
    case HaveSound:
    {
        for(int i=0;i<SoundList.length();i++)
        {
            if(SoundList.at(i)==true)
            {
                result=true;
                break;
            }
        }
        break;
    }
    case NOSound:
    {
        for(int i=0;i<SoundList.length();i++)
        {
            if(SoundList.at(i)==false)
            {
                result=true;
                break;
            }
        }
        break;
    }
    case HCountthanNCount:
    {
        int timer=0;
        for(int i=0;i<SoundList.length();i++)
        {
            if(SoundList.at(i)==true)
                timer++;
        }
        if(timer>ColSOUNDTimer/2)
            result=true;
        break;
    }
    case HCountlessNCount:
    {
        int timer=0;
        for(int i=0;i<SoundList.length();i++)
        {
            if(SoundList.at(i)==false)
                timer++;
        }
        if(timer>ColSOUNDTimer/2)
            result=true;
        break;
    }
    case noHSoundCount:
    {
        int i;
        for(i=0;i<SoundList.length();i++)
        {
            if(SoundList.at(i)==true)
                break;
        }

        if(i==SoundList.length())
            result=true;
        break;
    }

    case noNSoundCount:
    {
        int i;
        for(i=0;i<SoundList.length();i++)
        {
            if(SoundList.at(i)==false)
                break;
        }

        if(i==SoundList.length())
            result=true;
        break;
    }
    }
    QString souStr;
    for(int i=0;i<SoundList.length();i++)
    {
        souStr+= " "+toStr(SoundList.at(i));
    }

    appendTheResultToFile("Judge:Sound:"+getSoundJudge(sound.sound));
    appendTheResultToFile("Check:Sound:"+souStr);
    appendTheResultToFile("Result:Sound:"+toStr(result));

    return result;
}

/*************************************************************
/函数功能：获取log运行结果
/函数参数：log文件路径
/函数返回：检测结果：0-失败  1-成功
*************************************************************/
bool testUnit::chkScript(QString filePath,checkParam script)
{
    ShowList<<tr("checkTheAction:检测脚本...")+script.logContains;
    bool result = false;
    QFile readfile(filePath);

    if(readfile.open((QIODevice::ReadOnly|QIODevice::Text)))                           //打开
    {
        QTextStream in(&readfile);                                                     //定义传输流

        while(!in.atEnd())                                                             //文件内循环
        {
            if(in.readLine().contains(script.logContains))//"OK (1 test)"  //行判断
            {
               result = true;                                                          //查找到需要字符串
               break;
            }
        }
    }
    readfile.close();                                                                  //文件关闭

    appendTheResultToFile("Judge:Script: "+script.logContains);
    appendTheResultToFile("Check:Script:"+filePath);
    appendTheResultToFile("Result:Script:"+toStr(result));

    return result;
}

/*************************************************************
/函数功能：检测记忆
/函数参数：记忆检测参数
/函数返回：wu
*************************************************************/
bool testUnit::chkInterface(QString infoStr,checkParam memory)
{
    QString curFaceInfo,lastFaceInfo;
    bool result = false;

    if(infoStr.isEmpty()==false)
    {
        /*获取当前界面信息*/
        curFaceInfo = ColInfo_find(infoStr,tempFaceInfo);

        ShowList<< tr("checkTheAction:检测界面...")+curFaceInfo;

        /*根据比较添加进行界面检验*/
        if(curFaceInfo.isEmpty() == false)
        {
            if(memory.infoCompare == MemoryCompare)
            {
                lastFaceInfo = ColInfo_find(memory.comTarget,tempFaceInfo);

                if(lastFaceInfo.isEmpty()==false)
                {
                    if(lastFaceInfo == curFaceInfo)
                        result = true;
                }
                else
                    ShowList <<tr("Warn:未采集到动作执行前界面，检测失败！");
            }
            else if(memory.infoCompare == NoCompare)
            {
                result = true;//界面开启即为真
                lastFaceInfo = "Is Interface Start?";
            }
            else if(memory.infoCompare == SelfCompare)
            {

            }
            else
            {

            }
        }
        else
        {
            ShowList <<tr("Warn:未查询到当前界面，检测失败！");
        }
    }
    else
    {
        ShowList <<tr("Warn:未添加该检测的界面采集，请检查测试单元配置！");
        curFaceInfo = "未添加采集界面信息！";
    }

    appendTheResultToFile("Judge:Interface:"+lastFaceInfo);
    appendTheResultToFile("Check:Interface:"+curFaceInfo);
    appendTheResultToFile("Result:Interface:"+toStr(result));
    return result;
}

/*************************************************************
/函数功能：检测ADB命令下获取图片
/函数参数：动作定义
/函数返回：wu
/any：添加图片校验：类界面校验
*************************************************************/
bool testUnit::chkADBPic(QString infoStr, checkParam adbpic)
{
    QString curPicInfo,lastPicInfo;
    bool result = false;

    if(infoStr.isEmpty()==false)
    {
        /*获取当前界面信息*/
        curPicInfo = ColInfo_find(infoStr,tempPicInfo);

        ShowList<< tr("checkTheAction:检测Picture...")+curPicInfo;

        QFileInfo file(curPicInfo);

        /*根据比较添加进行界面检验*/
        if((curPicInfo.isEmpty() == false)&&(file.exists()))
        {
            Model_PicCompare picDeal;
            if(adbpic.infoCompare == MemoryCompare)
            {
                //查询之前对比界面，并比较
                lastPicInfo = ColInfo_find(adbpic.comTarget,tempPicInfo);

                QFileInfo filel(lastPicInfo);

                if((lastPicInfo.isEmpty()==false)&&(filel.exists()))
                    result = picDeal.Cameracompare(curPicInfo,lastPicInfo);//any:比较2图片的相似度
                else
                {
                    lastPicInfo = "Warn:未采集到动作执行前图片！";
                    ShowList <<tr("Warn:未采集到动作执行前图片，检测失败！");
                }

            }
            else if(adbpic.infoCompare == NoCompare)
            {
                result = true;//界面开启即为真
                lastPicInfo = "Is Face Start?";
            }
            else if(adbpic.infoCompare == SelfCompare)
            {
                for(int i=0;i<fixedInfo.length();i++)
                {
                    //查找同动作下采集的固定信息数据
                    if(fixedInfo.at(i).name == infoStr)
                    {
                        lastPicInfo = fixedInfo.at(i).information.toString();

                        QFileInfo filel(lastPicInfo);
                        if((lastPicInfo.isEmpty()==false)&&(filel.exists()))
                            result = picDeal.Cameracompare(curPicInfo,lastPicInfo);//any:比较2图片的相似度
                        else
                            ShowList <<tr("Warn:未采集到动作执行前图片，检测失败！");

                        break;
                    }
                }
            }
        }
        else
        {
            ShowList <<tr("Warn:未查询到当前图片，检测失败！");
            curPicInfo = "Warn:未查询到当前图片，检测失败！";
        }
    }
    else
    {
        ShowList <<tr("Warn:未添加该检测的图片采集，请检查测试单元配置！");
        curPicInfo = "未添加采集图片信息！";
    }

    appendTheResultToFile("Judge:Picture:"+lastPicInfo);
    appendTheResultToFile("Check:Picture:"+curPicInfo);
    appendTheResultToFile("Result:Picture:"+toStr(result));

    return result;
}

/*************************************************************
/函数功能：检测，，
/函数参数：动作定义
/函数返回：wu
*************************************************************/
bool testUnit::chkRes(checkParam res)
{
    Q_UNUSED(res);
    return false;
}

/*************************************************************
/函数功能：动作处理中采集信息读取
/函数参数：采集位置，信息内容  信息列表
/函数返回：无
//any:size:优化
*************************************************************/
QString testUnit::ActColInfo_Read(bool size,QString info,QStringList colInfoList)
{
    QString infoStr;
    for(int i=0;i<colInfoList.length();i++)
    {
        if(colInfoList.at(i).contains(info))
        {
            if((size == ACT_Front)&&(colInfoList.at(i).endsWith(":Front")))
                infoStr = colInfoList.at(i);
            else if((size == ACT_Back)&&(colInfoList.at(i).endsWith(":Back")))
                infoStr = colInfoList.at(i);
        }
    }
    return infoStr;
}

/*************************************************************
/函数功能：根据执行位置判断采集信息是否进行
/函数参数：当前位置
/函数返回：是否采集信息
*************************************************************/
uint16_t testUnit::ActColInfo_Analy(bool site,QStringList colInfoList)
{
    uint16_t actInfoFlag=0;

    if(colInfoList.isEmpty())
        return actInfoFlag;

    //colSize = site;
    //actInfoFlag = 0x00;
    //colInfoFlag=0x00;

    if(site == ACT_Front)
    {
        for(int i=0;i<colInfoList.length();i++)
        {
            QString infoStr = colInfoList.at(i);
            if(infoStr.contains("Interface:Front"))
                actInfoFlag|=SIZE_Interface;
            else if(infoStr.contains("Picture:Front"))
                actInfoFlag|=SIZE_Picture;
        }
    }
    else
    {
        for(int i=0;i<colInfoList.length();i++)
        {
            QString infoStr = colInfoList.at(i);
            if(infoStr.contains("Interface:Back"))
                actInfoFlag|=SIZE_Interface;
            else if(infoStr.contains("Picture:Back"))
                actInfoFlag|=SIZE_Picture;
            else if(infoStr.endsWith("Current"))
                actInfoFlag|=SIZE_Current;
            else if(infoStr.endsWith("Volt"))
                actInfoFlag|=SIZE_Volt;
            else if(infoStr.endsWith("Sound"))
                actInfoFlag|=SIZE_Sound;
        }
    }
    return actInfoFlag;
}

/*************************************************************
/函数功能：信息存储
/函数参数：信息类型  数据
/函数返回：无
*************************************************************/
void testUnit::ColInfo_Append(uint16_t infoflag,storageInfo_type_s infoDat,QList <checkParam> checkDeal)
{
    /*查找界面处理的判断条件*/
    for(int i=0;i<checkDeal.length();i++)
    {
        if(((infoflag == SIZE_Interface)&&(checkDeal.at(i).check == CHKInterface))
                ||((infoflag == SIZE_Picture)&&(checkDeal.at(i).check == CHKADBPIC)))
        {
            if(checkDeal.at(i).infoCompare == SelfCompare)
            {
                for(int j=0;j<fixedInfo.length();j++)
                {
                    /*自身比较界面：固定信息中存在该信息，添加为临时信息用来和固定信息进行比较*/
                    if(fixedInfo.at(j).name == infoDat.name)
                        goto AddTempInfo;
                }
                /*自身比较界面：固定信息中不存在该信息，添加为固定信息*/
                fixedInfo.append(infoDat);
            }
            else
                goto AddTempInfo;
        }
    }

    AddTempInfo:
    if(infoflag == SIZE_Picture)
        tempPicInfo.append(infoDat);
    else if(infoflag == SIZE_Interface)
        tempFaceInfo.append(infoDat);
}

/*************************************************************
/函数功能：采集信息查找
/函数参数：name：根据名字查找   infoDat：信息数据列表
/函数返回：wu
*************************************************************/
QString testUnit::ColInfo_find(QString name,QList <storageInfo_type_s> infoDat)
{
    QString findStr;
    /*获取信息*/
    for(int i=0;i<infoDat.length();i++)
    {
        if(infoDat.at(i).name == name)
        {
            findStr = infoDat.at(i).information.toString();
            break;
        }
    }
    return findStr;
}



/*************************************************************
/函数功能：初始化动作参数
/函数参数：动作
/函数返回：wu
*************************************************************/
void testUnit::inittActionParam(tAction *tact)
{
    tact->actName = "";
    tact->actStr =  "";
    tact->timeDeal.wait = 0;
    tact->timeDeal.check = 0;
    tact->timeDeal.end = 60000;

    //tact->infoFlag=0;
    tact->errorDeal=0;
    tact->colInfoList.clear();
    tact->checkDeal.clear();
    tact->changedDeal.clear();
}

/*************************************************************
/函数功能：动作添加ACCON
/函数参数：无
/函数返回：wu
*************************************************************/
void testUnit::actAppend_ACCON(int actNum,tAction *kAction,QList <keyControl> keyList)
{
    //tAction kAction;
    inittActionParam(kAction);
    kAction->actName = "ACCON";
    kAction->actFlag = ACT_KEY;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardACC))
        {
            kAction->actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name+":on";
            break;
        }
    }

    kAction->timeDeal.wait = 5000;
    kAction->timeDeal.check = 0;
    kAction->timeDeal.end = 60000;

    checkParam checkCurrent;
    initNullChkParam(&checkCurrent);
    checkCurrent.check = CHKCurrent;
    checkCurrent.range = GE;
    checkCurrent.min = WorkCurrent;
    kAction->checkDeal.append(checkCurrent);

   /* checkParam checkMemoey;
    initNullChkParam(&checkMemoey);
    checkMemoey.check = CHKInterface;
    checkMemoey.infoCompare = MemoryCompare;
    kAction.checkDeal.append(checkMemoey);*/

    //kAction.colInfoList.append("ACT"+toStr(row+1)+":Interface:Back");
    kAction->colInfoList.append("ACT"+toStr(actNum)+":Current");

    /*checkParam checkSound;
    initNullChkParam(&checkSound);
    checkSound.check = CHKSound;
    checkSound.sound = noNSoundCount;
    kAction.checkDeal.append(checkSound);*/

    //appendTableAction(kAction);
}

/*************************************************************
/函数功能：动作添加ACCOFF
/函数参数：无
/函数返回：wu
*************************************************************/
void testUnit::actAppend_ACCOFF(int actNum,tAction *kAction,QList <keyControl> keyList)
{
    inittActionParam(kAction);
    kAction->actName = "ACCOFF";
    kAction->actFlag = ACT_KEY;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardACC))
        {
            kAction->actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name+":off";
            break;
        }
    }

    kAction->timeDeal.wait = 10000;
    kAction->timeDeal.check = 15000;
    kAction->timeDeal.end = 60000;

    kAction->colInfoList.append("ACT"+toStr(actNum)+":Interface:Front");
    kAction->colInfoList.append("ACT"+toStr(actNum)+":Current");

    changedParam changeTime;
    changeTime.changed = WaitTime;
    changeTime.dir = true;
    changeTime.min  = kAction->timeDeal.wait;
    changeTime.max  = 30000;
    changeTime.step = 100;
    kAction->changedDeal.append(changeTime);

    checkParam checkCurrent;
    initNullChkParam(&checkCurrent);
    checkCurrent.check = CHKCurrent;
    checkCurrent.range = LE;
    checkCurrent.min = 20;
    kAction->checkDeal.append(checkCurrent);
}

/*************************************************************
/函数功能：动作添加BATON
/函数参数：无
/函数返回：wu
*************************************************************/
void testUnit::actAppend_BATON(int actNum,tAction *kAction,QList <keyControl> keyList)
{
    inittActionParam(kAction);
    kAction->actName = "BATON";
    kAction->actFlag = ACT_KEY;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardBAT))
        {
            kAction->actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name+":on";
            break;
        }
    }

    kAction->timeDeal.wait = 10000;
    kAction->timeDeal.check = 0;
    kAction->timeDeal.end = 60000;


    checkParam checkCurrent;
    initNullChkParam(&checkCurrent);
    checkCurrent.check = CHKCurrent;
    checkCurrent.range = GE;
    checkCurrent.min = WorkCurrent;
    kAction->checkDeal.append(checkCurrent);

   /* checkParam checkMemoey;
    initNullChkParam(&checkMemoey);
    checkMemoey.check = CHKInterface;
    checkMemoey.infoCompare = MemoryCompare;
    kAction.checkDeal.append(checkMemoey);*/

    //kAction.colInfoList.append("ACT"+toStr(row+1)+":Interface:Back");
    kAction->colInfoList.append("ACT"+toStr(actNum)+":Current");

    /*checkParam checkSound;
    initNullChkParam(&checkSound);
    checkSound.check = CHKSound;
    checkSound.sound = noNSoundCount;
    kAction.checkDeal.append(checkSound);*/

}

/*************************************************************
/函数功能：动作添加BATOFF
/函数参数：无
/函数返回：wu
*************************************************************/
void testUnit::actAppend_BATOFF(int actNum,tAction *kAction,QList <keyControl> keyList)
{
    inittActionParam(kAction);
    kAction->actName = "BATOFF";
    kAction->actFlag = ACT_KEY;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardBAT))
        {
            kAction->actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name+":off";
            break;
        }
    }

    kAction->timeDeal.wait = 5000;
    kAction->timeDeal.check = 0;
    kAction->timeDeal.end = 60000;

    kAction->colInfoList.append("ACT"+toStr(actNum)+":Interface:Front");

    changedParam changeTime;
    changeTime.changed = WaitTime;
    changeTime.dir = true;
    changeTime.min  = kAction->timeDeal.wait;
    changeTime.max  = 30000;
    changeTime.step = 100;
    kAction->changedDeal.append(changeTime);
}

/*************************************************************
/函数功能：动作添加CCDON
/函数参数：无
/函数返回：wu
*************************************************************/
void testUnit::actAppend_CCDON(int actNum,tAction *kAction,QList <keyControl> keyList)
{
    inittActionParam(kAction);
    kAction->actName = "CCDON";
    kAction->actFlag = ACT_KEY;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardCCD))
        {
            kAction->actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name+":on";
            break;
        }
    }

    kAction->timeDeal.wait = 5000;
    kAction->timeDeal.check = 0;
    kAction->timeDeal.end = 60000;

    kAction->colInfoList.append("ACT"+toStr(actNum)+":Interface:Front");
    kAction->colInfoList.append("ACT"+toStr(actNum)+":Picture:Back");

    changedParam changeTime;
    changeTime.changed = WaitTime;
    changeTime.dir = true;
    changeTime.min  = kAction->timeDeal.wait;
    changeTime.max  = 30000;
    changeTime.step = 100;
    kAction->changedDeal.append(changeTime);

    checkParam checkPicture;
    initNullChkParam(&checkPicture);
    checkPicture.check = CHKADBPIC;
    checkPicture.infoCompare = SelfCompare;
    kAction->checkDeal.append(checkPicture);
}

/*************************************************************
/函数功能：动作添加CCDOFF
/函数参数：无
/函数返回：wu
*************************************************************/
void testUnit::actAppend_CCDOFF(int actNum,tAction *kAction,QList <keyControl> keyList)
{
    inittActionParam(kAction);
    kAction->actName = "CCDOFF";
    kAction->actFlag = ACT_KEY;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardCCD))
        {
            kAction->actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name+":off";
            break;
        }
    }

    kAction->timeDeal.wait = 5000;
    kAction->timeDeal.check = 0;
    kAction->timeDeal.end = 60000;

    /*
    checkParam checkMemoey;
    initNullChkParam(&checkMemoey);
    checkMemoey.check = CHKInterface;
    checkMemoey.infoCompare = MemoryCompare;
    kAction.checkDeal.append(checkMemoey);

    int row=ui->tableAction->rowCount();
    kAction.colInfoList.append("ACT"+toStr(row+1)+":Interface:Back");
*/
    /*checkParam checkSound;
    initNullChkParam(&checkSound);
    checkSound.check = CHKSound;
    checkSound.sound = noNSoundCount;
    kAction.checkDeal.append(checkSound);*/

}

/*************************************************************
/函数功能：动作添加key
/函数参数：无
/函数返回：wu
*************************************************************/
void testUnit::actAppend_key(QString keyName,tAction *kAction,QList <keyControl> keyList)
{
    inittActionParam(kAction);

    kAction->actFlag = ACT_KEY;
    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).name == keyName))
        {
            if((keyList.at(i).type == Can1_1)||(keyList.at(i).type == Can2_1))
            {
                kAction->actName = keyName;
                kAction->actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name;
            }
            else
            {
                kAction->actName = keyName+"-OFF";
                kAction->actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name+":off";
            }

            kAction->timeDeal.end = 60000;

            break;
        }
    }

}

/*************************************************************
/函数功能：动作添加script
/函数参数：无
/函数返回：wu
*************************************************************/
void testUnit::actAppend_script(tAction *sAction)
{
    inittActionParam(sAction);

    sAction->actName="Script";
    sAction->actFlag = ACT_SCRIPT;

    sAction->timeDeal.end = 60000;

    //创建脚本检测
    checkParam checkScript;
    initNullChkParam(&checkScript);
    checkScript.check = CHKScript;
    checkScript.logContains = "OK (1 test)";
    sAction->checkDeal.append(checkScript);
}

/*************************************************************
/函数功能：动作添加batVolt
/函数参数：无
/函数返回：wu
*************************************************************/
void testUnit::actAppend_batVolt(tAction *bAction)
{
    inittActionParam(bAction);

    bAction->actName="BATVolt";
    bAction->actFlag = ACT_BATVolt;
    bAction->actStr = "BAT:Volt:10";

    changedParam changevolt;
    changevolt.changed = BatVolt;
    changevolt.dir = true;
    changevolt.min  = 10;
    changevolt.max  = 15;//any:暂时使用
    changevolt.step = 1;
    bAction->changedDeal.append(changevolt);
}

