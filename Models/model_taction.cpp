#include "model_taction.h"


QList <storageInfo_type_s> fixedInfo;       //固定界面信息：随测试序列刷新：即每次开始测试启动时清空，测试过程中首次测试某一项，添加到列表中，之后的测试单元测试时将于其相对应数据比较

QList <storageInfo_type_s> tempFaceInfo;        //临时界面信息：随测试单元刷新：即测试单元开始可填写数据，结束即清空
QList <storageInfo_type_s> tempPicInfo;         //临时图片信息

QList <bool> tempSoundInfo;                        //定义临时声音信息：用来处理声音临时数据存储



/* this is Action Deal
 * ：测试流程改为：关ACC--开ACC--脚本（优点，测试单元执行结束机器仍为正常工作状态，无需恢复；测试记忆功能也可在一个测试单元中处理）
 * 增加任务：
 * 3.BAT测试增加改变电压的测试；
 * 4.动作执行完成处理
*/
Model_tAction::Model_tAction(int loop,tAction *Action)
{
    iniLoop = loop;
    actionDeal = Action;
    timeState = start;
    timeID_T = startTimer(1);//开启定时器
    initProcessDeal();
}

Model_tAction::~Model_tAction()
{
    deleteProcessDeal();
}

/*************************************************************
/函数功能：定时器事件，处理测试
/函数参数：事件
/函数返回：无
*************************************************************/
void Model_tAction::timerEvent(QTimerEvent *event)
{
    //处理计时
    if(event->timerId()==timeID_T)
    {
        switch (timeState)
        {
        case start:
        {
            //初始化变量：
            testResult = false;     //:通常超时为测试失败,因此模式测试失败
            timeCount=0;
            reChkCount=0;
            overtimeAct=0;
            IsFirstMemory=true;
            actIsRunning=false;
            colInfoFlag=0;
            infoFlag = actionDeal->infoFlag & 0x0F;

            tempSoundInfo.clear();

            //显示执行且保存到结果文件：
            ShowList << "evaluateTheAction:"+actionDeal->actName;
            appendTheResultToFile("evaluateTheAction:"+actionDeal->actName);

            //判断动作执行前是否采集信息：
            if(judgeIsCollectInfo(ACT_Front))
            {
                timeState = collectInfo;
                nextState = exeAction;
                ShowList << "采集信息：";
            }
            else
                timeState = exeAction;

            break;
        }
        case exeAction://执行动作并判断执行结束
        {
            if((!actIsRunning)&&(!overtimeAct))
            {
                ShowList << "~执行动作~";
                if(actionDeal->actStr.startsWith("KEY"))
                    startAction(actionDeal->actStr);//执行按键动作
                else
                {
                    if((!isPRORunning)&&(proList.isEmpty()))
                        onProcessEXECmd(CMD_script);
                }
                actIsRunning=true;
            }
            else
            {
                /*等待动作处理完成时，会将标志位复位*/
                if(!actIsRunning)
                {
                    if(actionDeal->timeDeal.wait)
                    {
                        ShowList << ("waitTime："+toStr(actionDeal->timeDeal.wait)+"mS");
                        timeState = wait;
                    }
                    else
                        timeState = waitover;
                }
                else
                {
                    if(overtimeAct++ > 60000)
                        timeState = actover;//动作执行超时
                    else
                    {
                        //脚本运行连续执行
                        if((actionDeal->actStr.startsWith("KEY")==false)&&(!isPRORunning)&&(proList.isEmpty()))
                            onProcessEXECmd(CMD_script);
                    }
                }
            }
            break;
        }
        case chkAction:
        {
            clearAction();//在结果处理时有对记忆进行赋值，若开始判断结果时将清除数据，避免对后续操作影响
            theActionCheckReault(actionDeal->checkDeal);//检测
            timeState = actover;
            break;
        }
        case collectInfo:
        {
            collectInfoDeal(infoFlag);
            break;
        }
        case wait://计数并判断时间
        {
            timeCount++;

            /*未结束等待前检测到应该检测数据，判断数据的结果*/
            if(timeCount == actionDeal->timeDeal.check)
            {
                theActionCheckReault(actionDeal->checkDeal);
            }

            /* 结束等待时，若检测时间不存在将默认进入等待结束，
             * 若存在上述语句已经执行，因此直接进入actover操作*/
            if(timeCount == actionDeal->timeDeal.wait)
            {
                if(!actionDeal->timeDeal.check)//无检测时间
                    timeState = waitover;
                else
                {
                    testResult =true;
                    timeState = actover;
                }
            }
            break;
        }
        case waitover:
        {
            //判断动作执行后是否采集信息：
            if(judgeIsCollectInfo(ACT_Back))
            {
                timeState = collectInfo;
                nextState = chkAction;
                ShowList << "采集信息：";
            }
            else
                timeState = chkAction;
            break;
        }
        case actover:
        {//结束
            theActionChangedDeal(actionDeal->changedDeal);
            theActionOverTest(testResult);
            killTimer(timeID_T);
            break;
        }
        }
    }
    else if(event->timerId() == timerProID)
    {
        timerProIDDeal();
    }
}



/*************************************************************
/函数功能：根据执行位置判断采集信息是否进行
/函数参数：当前位置
/函数返回：是否采集信息
*************************************************************/
bool Model_tAction::judgeIsCollectInfo(bool site)
{
    if(site == ACT_Front)
    {
        //判断动作执行前是否采集信息,并填写要采集的标志位：
        if(((actionDeal->infoFlag & COLFACE) && (!(actionDeal->infoFlag & COLFACESITE)))
                || ((actionDeal->infoFlag & COLPICTURE) && (!(actionDeal->infoFlag & COLPICTURESITE))))
        {
            if((actionDeal->infoFlag & COLFACE)&&(!(actionDeal->infoFlag & COLFACESITE)))
                infoFlag|=COLFACE;
            if((actionDeal->infoFlag & COLPICTURE)&&(!(actionDeal->infoFlag & COLPICTURESITE)))
                infoFlag|=COLPICTURE;
            return true;
        }
        else
            return false;
    }
    else
    {
        //判断动作执行后是否采集信息：
        if(((actionDeal->infoFlag & COLFACE) && ((actionDeal->infoFlag & COLFACESITE)))
                || ((actionDeal->infoFlag & COLPICTURE) && ((actionDeal->infoFlag & COLPICTURESITE))))
        {
            if((actionDeal->infoFlag & COLFACE)&&(actionDeal->infoFlag & COLFACESITE))
                infoFlag|=COLFACE;
            if((actionDeal->infoFlag & COLPICTURE)&&(actionDeal->infoFlag & COLPICTURESITE))
                infoFlag|=COLPICTURE;
            return true;
        }
        else
            return false;
    }

}

/*************************************************************
/函数功能：根据执行位置判断采集信息是否进行
/函数参数：当前位置
/函数返回：是否采集信息
/采集优先级顺序：电流--界面--图片--声音(依据开机参数能获取的顺序)
*************************************************************/
void Model_tAction::collectInfoDeal(uint16_t infoFlag)
{
    //采集电流：
    if((!(colInfoFlag & COLCURRENT))&&(infoFlag & COLCURRENT))
    {
        //处理的时间超过检测时间方可检测数据：
        if(actionDeal->timeDeal.wait > actionDeal->timeDeal.check)
        {
            for(int i=0;i<actionDeal->checkDeal.length();i++)
            {
                checkParam chkDeal = actionDeal->checkDeal.at(i);
                if(chkDeal.check == CHKCurrent)
                {
                    if(rangeJudgeTheParam(chkDeal.range,chkDeal.min,chkDeal.max,Current)==false)
                        goto toContinue;

                    colInfoFlag |= COLCURRENT;      //电流采集完成
                }
            }
        }
        else
            colInfoFlag |= COLCURRENT;      //电流采集完成
    }
    //采集界面：
    if((!(colInfoFlag & COLFACE))&&(infoFlag & COLFACE))
    {
        if((!isPRORunning)&&(proList.isEmpty()))
            onProcessEXECmd(CMD_FACE);

        goto toContinue;
    }
    //采集图片：
    if((!(colInfoFlag & COLPICTURE))&&(infoFlag & COLPICTURE))
    {
        if((!isPRORunning)&&(proList.isEmpty()))
            onProcessEXECmd(CMD_ADBPic);

        goto toContinue;
    }
    //采集声音：
    if((!(colInfoFlag & COLSOUND))&&(infoFlag & COLSOUND))
    {

    }

    //信息采集完成，执行下一步：
    //将标志位置的位清0
    infoFlag &= ~(1<<5);
    infoFlag &= ~(1<<7);
    if(colInfoFlag == infoFlag)
        timeState=nextState;

    toContinue:
    {
        //添加超时处理机制：
        if(reChkCount >= actionDeal->timeDeal.end)
        {
            ShowList <<"Warn:采集信息超时；";
            timeState=nextState;
        }
        reChkCount++;
    }
}

/*************************************************************
/函数功能：信息存储
/函数参数：信息类型  数据
/函数返回：无
*************************************************************/
void Model_tAction::infoAppendDeal(uint16_t infoflag,storageInfo_type_s infoDat)
{
    /*查找界面处理的判断条件*/
    for(int i=0;i<actionDeal->checkDeal.length();i++)
    {
        if(((infoflag == COLFACE)&&(actionDeal->checkDeal.at(i).check == CHKInterface))
                ||((infoflag == COLPICTURE)&&(actionDeal->checkDeal.at(i).check == CHKADBPIC)))
        {
            if(actionDeal->checkDeal.at(i).infoCompare == SelfCompare)
            {
                for(int j=0;j<fixedInfo.length();j++)
                {
                    /*自身比较界面：固定信息中存在该信息，添加为临时信息用来和固定信息进行比较*/
                    if(fixedInfo.at(j).name == infoDat.name)
                        goto AddTempInfo;
                }
                /*自身比较界面：固定信息中不存在该信息，添加为固定信息*/
                if(infoflag == COLPICTURE)
                {
                    QString firstFile = actionDeal->actName+QDateTime::currentDateTime().toString("yyyyMMddhhmmss")+".png";
                    proList.append(RENAME(infoDat.information.toString(),firstFile));
                    infoDat.information =TEMPPath + firstFile;
                }
                fixedInfo.append(infoDat);
            }
            else
                goto AddTempInfo;
        }
    }

    AddTempInfo:
    if(infoflag == COLPICTURE)
        tempPicInfo.append(infoDat);
    else if(infoflag == COLFACE)
        tempFaceInfo.append(infoDat);
}

/*************************************************************
/函数功能：测试变动处理
/函数参数：测试变动参数
/函数返回：wu
*************************************************************/
void Model_tAction::theActionChangedDeal(QList <changedParam>testChanged)
{
    if(testChanged.isEmpty())
        return ;

    for(int i=0;i<testChanged.length();i++)
    {
        changedParam cngDeal = testChanged.at(i);
        switch(cngDeal.changed)
        {
        case WaitTime:
        {
            if(cngDeal.dir)
            {
                //递增
                if((actionDeal->timeDeal.wait)<cngDeal.max)
                {
                    if((actionDeal->timeDeal.wait+cngDeal.step)<cngDeal.max)
                        actionDeal->timeDeal.wait = actionDeal->timeDeal.wait+cngDeal.step;
                    else
                        actionDeal->timeDeal.wait = cngDeal.max;
                }
                else
                    actionDeal->timeDeal.wait = cngDeal.min;
            }
            else
            {
                //递减
                if((actionDeal->timeDeal.wait)>cngDeal.min)
                {
                    if((actionDeal->timeDeal.wait-cngDeal.step)>cngDeal.min)
                        actionDeal->timeDeal.wait = actionDeal->timeDeal.wait-cngDeal.step;
                    else
                        actionDeal->timeDeal.wait = cngDeal.min;
                }
                else
                    actionDeal->timeDeal.wait = cngDeal.max;
            }
            break;
        }
        case BatVolt:
        {
            break;
        }
        }
    }

}

/*************************************************************
/函数功能：检测动作执行结果:请求结果检测：实际的检测结果另行处理
/函数参数：测试检测参数
/函数返回：无
*************************************************************/
void Model_tAction::theActionCheckReault(QList <checkParam> testChk)
{
    if(testChk.isEmpty())
    {
        testResult = true;//当前无检测项，判断结果为真
        return ;
    }
    bool result = true;

    for(int i=0;i<testChk.length();i++)
    {
        switch(testChk.at(i).check)
        {
        case CHKCurrent:
        result &= chkCurrent(testChk.at(i));
        break;
        case CHKVlot:
        result &= chkCurrent(testChk.at(i));
        break;
        case CHKSound:
        result &= chkSound(testChk.at(i));
        break;
        case CHKScript:
        result &= chkScript(testChk.at(i));
        break;
        case CHKInterface:
        result &= chkInterface(testChk.at(i));
        break;
        case CHKRES:
        result &= chkRes(testChk.at(i));
        break;
        case CHKADBPIC:
        result &= chkADBPic(testChk.at(i));
        break;
        }
    }
    testResult = result;
}

/*************************************************************
/函数功能：检测电流
/函数参数：范围检测参数
/函数返回：wu
*************************************************************/
bool Model_tAction::chkCurrent(checkParam range)
{
    int value = Current;
    ShowList << "checkTheAction:检测电流..."+toStr(value);
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
bool Model_tAction::chkVolt(checkParam range)
{
    int value = Volt;
    ShowList << "checkTheAction:检测电压..."+toStr(value);
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
bool Model_tAction::chkSound(checkParam sound)
{
    ShowList<< "checkTheAction:检测声音...";
    return false;
}

/*************************************************************
/函数功能：获取log运行结果
/函数参数：log文件路径
/函数返回：检测结果：0-失败  1-成功
*************************************************************/
bool Model_tAction::chkScript(checkParam script)
{
    ShowList<<("checkTheAction:检测脚本...");
    bool result = false;
    QString filePath = savePath+"\\"+toStr(iniLoop)+"\\case.log";            //any--检测脚本路径
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
bool Model_tAction::chkInterface(checkParam memory)
{
    QString curFaceInfo,lastFaceInfo;
    bool result = false;

    ShowList<< "checkTheAction:检测界面...";

    /*获取当前界面信息*/
    for(int i=0;i<tempFaceInfo.length();i++)
    {
        if(tempFaceInfo.at(i).name.contains(actionDeal->actStr) && tempFaceInfo.at(i).name.contains("-FACE"))
        {
            curFaceInfo = tempFaceInfo.at(i).information.toString();
        }
    }

    /*根据比较添加进行界面检验*/
    if(curFaceInfo.isEmpty() == false)
    {
        if(memory.infoCompare == MemoryCompare)
        {
            //查询之前对比界面，并比较
            bool curStatus ;

            if(actionDeal->actStr.contains(":on"))
                curStatus=true;
            else
                curStatus=false;

            //同时查找该动作之前测试单元中前一个状态下 添加上动作执行前采集当前界面信息
            for(int i=0;i<tempFaceInfo.length();i++)
            {
                QString actStr=actionDeal->actStr;
                QString unitfindStr = tempFaceInfo.at(i).name;

                if(((curStatus)&&((unitfindStr.contains(":off"))&&(unitfindStr.contains(actStr.remove(":on")))))
                 ||((!curStatus)&&((unitfindStr.contains(":on"))&&(unitfindStr.contains(actStr.remove(":off"))))))
                {
                    lastFaceInfo = tempFaceInfo.at(i).information.toString();
                }
            }

            if(lastFaceInfo.isEmpty()==false)
            {
                if(lastFaceInfo == curFaceInfo)
                    result = true;
            }
            else
            {
                ShowList <<"Warn:未采集到动作执行前界面，检测失败！";
            }
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
        ShowList <<"Warn:未查询到当前界面，检测失败！";
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
bool Model_tAction::chkADBPic(checkParam adbpic)
{
    QString curPicInfo,lastPicInfo;
    bool result = false;

    ShowList<< "checkTheAction:检测Picture...";

    /*获取当前界面信息*/
    for(int i=0;i<tempPicInfo.length();i++)
    {
        if(tempPicInfo.at(i).name.contains(actionDeal->actStr) && tempPicInfo.at(i).name.contains("-PICTURE"))
        {
            curPicInfo = tempPicInfo.at(i).information.toString();
        }
    }

    /*根据比较添加进行界面检验*/
    if(curPicInfo.isEmpty() == false)
    {
        //Model_PicCompare picDeal;
        if(adbpic.infoCompare == MemoryCompare)
        {
            //查询之前对比界面，并比较
            bool curStatus ;

            if(actionDeal->actStr.contains(":on"))
                curStatus=true;
            else
                curStatus=false;

            //同时查找该动作之前测试单元中前一个状态下 添加上动作执行前采集当前界面信息
            for(int i=0;i<tempPicInfo.length();i++)
            {
                QString actStr=actionDeal->actStr;
                QString unitfindStr = tempPicInfo.at(i).name;

                if(((curStatus)&&((unitfindStr.contains(":off"))&&(unitfindStr.contains(actStr.remove(":on")))))
                 ||((!curStatus)&&((unitfindStr.contains(":on"))&&(unitfindStr.contains(actStr.remove(":off"))))))
                {
                    lastPicInfo = tempPicInfo.at(i).information.toString();
                }
            }
            if(lastPicInfo.isEmpty()==false)
            {
                //any:比较2图片的相似度
                if(lastPicInfo == curPicInfo)
                    result = true;
                //picDeal.Cameracompare(curPicInfo,lastPicInfo);
            }
            else
            {
                ShowList <<"Warn:未采集到动作执行前界面，检测失败！";
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
                if(fixedInfo.at(i).name.contains(actionDeal->actStr) && fixedInfo.at(i).name.contains("-PICTURE"))
                {
                    lastPicInfo = fixedInfo.at(i).information.toString();
                    //picDeal.Cameracompare(curPicInfo,lastPicInfo);
                    break;
                }
            }
        }
    }
    else
    {
        ShowList <<"Warn:未查询到当前界面，检测失败！";
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
bool Model_tAction::chkRes(checkParam res)
{
    Q_UNUSED(res);
    return false;
}


/*---------------------------------------this is Process option-----------------------------------------*/
/*************************************************************
/函数功能：初始化进程处理
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tAction::initProcessDeal()
{
    PRODeal = new Model_Process;
    connect(PRODeal,SIGNAL(ProcessisOver(uint8_t)),this,SLOT(onProcessOverSlot(uint8_t)));
    connect(PRODeal,SIGNAL(ProcessOutDeal(int,QString)),this,SLOT(onProcessOutputSlot(int,QString)));

    PRODeal->ProcessPathJump(QCoreApplication::applicationDirPath());
    timerProID = startTimer(1);
    isPRORunning=false;
    proCMD = CMD_NULL;
}

/*************************************************************
/函数功能：释放进程处理
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tAction::deleteProcessDeal()
{
    killTimer(timerProID);
    delete PRODeal;
}

/*************************************************************
/函数功能：进程执行处理
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tAction::timerProIDDeal()
{
    if((proList.isEmpty()==false)&&(!proSysIsRunning()))
    {
        //cout << proList.length();
        PRODeal->ProcessStart(PROSYS,proList.first());

        isPRORunning=true;
        currentCMDString = proList.first();
        proList.removeFirst();
    }
}

/*************************************************************
/函数功能：进程执行命令：先执行adb devices  执行结束后根据proCMD来执行对应的命令，为保证设备在线再执行
/函数参数：需要执行的命令类型
/函数返回：无
*************************************************************/
void Model_tAction::onProcessEXECmd(cmd_type_e cmdType)
{
    //先扫描设备，后运行命令
    proList.append(ADBDevs);
    proCMD = cmdType;
}

/*************************************************************
/函数功能：进程输出槽函数处理
/函数参数：进程号  字符串
/函数返回：无
*************************************************************/
void Model_tAction::onProcessOutputSlot(int pNum,QString String)
{
    //进程处理
    if((pNum==PROSYS)&&(String.isEmpty()==false))
    {
        if(currentCMDString == ADBDevs)
        {
            QString DEVString = String;
            deviceList = DEVString.split("\r\n");

            if(deviceList.isEmpty()==false)
                deviceList.removeFirst();
        }
        else
        {
            if(proCMD == CMD_script)
            {
                //any:Error:执行脚本时获取部分显示错误的信息，用于处理，以及后期显示执行按键的信息；
            }
            else if(proCMD == CMD_FACE)
            {
                /*处理界面采集信息*/
                if(String.contains("mFocusedActivity: ActivityRecord"))
                {
                    QString faceStr = String;
                    int startIndex=faceStr.indexOf("com.");
                    storageInfo_type_s infoStorage;
                    infoStorage.name = actionDeal->actStr+"-FACE";
                    infoStorage.information = faceStr.mid(startIndex).remove("}\r\r\n");
                    infoAppendDeal(COLFACE,infoStorage);
                }
            }
        }
        ShowList << String;
    }
}

/*************************************************************
/函数功能：进程结束处理
/函数参数：进程号
/函数返回：无
/any:Error-adb devices扫描后执行命令，会出现多次命令同时执行问题，因标志位清空，定时器再次处理
*************************************************************/
void Model_tAction::onProcessOverSlot(uint8_t pNum)
{
    if(pNum==PROSYS)
    {
        if(currentCMDString == ADBDevs)
        {
            if(deviceList.isEmpty()==false)
            {
                for(int i=0;i<deviceList.length();i++)
                {
                    QString tempString = deviceList.at(i);
                    if((tempString.contains(getDevNumber()))&&(tempString.contains("\tdevice")))
                    {
                        if(getDevNumber().isEmpty())
                        {
                            if(proCMD == CMD_script)
                                proList.append(actionDeal->actStr + " " +savePath.replace("/","\\")+"\\"+toStr(iniLoop)+" "+getDevNumber());//进程命令运行
                            else if(proCMD == CMD_FACE)
                                proList.append(ACTIVITYFACE);
                            else if(proCMD == CMD_ADBPic)
                                proList.append(SCREENCAP(actionDeal->actName+".png"));
                        }
                        else
                        {
                            if(proCMD == CMD_script)
                                proList.append(actionDeal->actStr + " " +savePath.replace("/","\\")+"\\"+toStr(iniLoop)+" "+getDevNumber());//进程命令运行
                            else if(proCMD == CMD_FACE)
                                proList.append(ACTIVITYFACE_S(getDevNumber()));
                            else if(proCMD == CMD_ADBPic)
                                proList.append(SCREENCAP_S(getDevNumber(),actionDeal->actName+".png"));
                        }

                        break;
                    }
                }
            }
        }
        else
        {
            if(proCMD == CMD_script)
            {
                //测试脚本运行：any:Error-脚本运行未结束时赋值，照成下一步的关执行失效
                if(actIsRunning)
                {
                    actIsRunning=false;
                }
            }
            else if(proCMD == CMD_FACE)
            {
                colInfoFlag |= COLFACE;      //界面采集完成
            }
            else if(proCMD == CMD_ADBPic)
            {
                if(currentCMDString.contains("screencap"))
                {
                    QDir dir(TEMPPath);
                    if(!dir.exists())
                    {
                        if(dir.mkpath(TEMPPath) == false) //创建多级目录
                        {
                            cout << "创建属性文件夹失败！创建路径为："<<TEMPPath;
                            return ;
                        }
                    }
                    if(getDevNumber().isEmpty())
                        proList.append(PULLFile(actionDeal->actName+".png",TEMPPath));
                    else
                        proList.append(PULLFile_S(getDevNumber(),actionDeal->actName+".png",TEMPPath));
                    goto ToEndProcess;//跳转到进程结束处理，因为该多命令组合未完全执行结束，因此proCMD不清空
                }
                else if(currentCMDString.contains("pull"))
                {
                    QString picFile = TEMPPath+actionDeal->actName+".png";
                    storageInfo_type_s infoStorage;
                    infoStorage.name = actionDeal->actStr+"-PICTURE";
                    infoStorage.information = picFile.replace("/","\\");
                    infoAppendDeal(COLPICTURE,infoStorage);
                    colInfoFlag |= COLPICTURE;      //界面采集完成
                }
            }
            //恢复状态
            proCMD = CMD_NULL;
        }
        ToEndProcess:
        isPRORunning=false;
        currentCMDString.clear();
    }
}

/*************************************************************
/函数功能：停止系统进程执行logcat
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tAction::proStopSysLogcat()
{
    PRODeal->stopProcess(getDevNumber(),"logcat");
}

/*************************************************************
/函数功能：停止系统进程执行uiautomator
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tAction::proStopSysUiautomator()
{
    PRODeal->stopProcess(getDevNumber(),"uiautomator");
}

/*************************************************************
/函数功能：判断系统进程是否正在执行
/函数参数：无
/函数返回：无
/备注：判断的只是bat中某一条指令是否执行并不能判断bat执行是否正在运行
*************************************************************/
bool Model_tAction::proSysIsRunning()
{
    if(PRODeal->GetProcessRunStatus(PROSYS) != noRun)
        return true;
    else
        return false;
}






