#include "model_taction.h"




typedef struct
{
    int Current;
    int volt;

    QList <bool> tempSoundInfo;                        //定义临时声音信息：用来处理声音临时数据存储
}colInfoDat_s;

colInfoDat_s colInfoDat;

/* this is Action Deal
 * ：测试流程改为：关ACC--开ACC--脚本（优点，测试单元执行结束机器仍为正常工作状态，无需恢复；测试记忆功能也可在一个测试单元中处理）
 * 增加任务：
 * 3.BAT测试增加改变电压的测试；
 * 4.动作执行完成处理
*/
Model_tAction::Model_tAction(int loop,tAction *Action)
{
    unitDeal =new testUnit();

    iniLoop = loop;
    actionDeal = Action;

    theActionInitParam();
    initProcessDeal();

    timeID_T = startTimer(1,Qt::PreciseTimer);//开启定时器,并使用精确定时器
}

Model_tAction::~Model_tAction()
{
    delete unitDeal;
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
            theActionStartDeal();
            break;
        case exeAction://执行动作并判断执行结束
            theActionExecuateDeal();
            break;
        case chkAction:
            //clearAction();//在结果处理时有对记忆进行赋值，若开始判断结果时将清除数据，避免对后续操作影响
            theActionCheckReault(actionDeal->checkDeal);//检测
            break;
        case collectInfo:
        {
            if((!TimeDelay1S)||(TimeDelay1S % 1000 == 0))
                theActionCollectInfoDeal(colSize,actInfoFlag);
            TimeDelay1S++;

            //添加超时处理机制：
            if(reChkCount >= (actionDeal->timeDeal.end ))/// 1000
            {
                ShowList <<tr("Error:采集信息超时；");
                if(actionDeal->errorDeal == OVERTIMEERR)
                {
                    lastState = nextState;
                    timeState = pauseState;
                    PauseState = true;
                }
                else
                    timeState=nextState;
            }
            reChkCount++;

            break;
        }
        case wait://计数并判断时间
        {
            uint64_t elapsed = tim_WaitStart.msecsTo(QTime::currentTime());

            //cout<< tim_WaitLast << elapsed;
            if(actionDeal->actFlag == ACT_DELAYTime)
            {
                uint64_t delayTime = actionDeal->actStr.toInt();

                if(elapsed >= delayTime)
                {
                    // cout << delayTime;
                    testResult =true;
                    timeState = actover;
                }
            }
            else
            {
                /*未结束等待前检测到应该检测数据，判断数据的结果*/
                if((actionDeal->timeDeal.check)&&(tim_WaitLast < actionDeal->timeDeal.check)&&(elapsed >= actionDeal->timeDeal.check))
                {
                    qDebug()<<"QTime.currentTime ="<<elapsed<<"ms";
                    colSize=ACT_Back;
                    colInfoFlag=0x00;
                    actInfoFlag = unitDeal->ActColInfo_Analy(ACT_Back,actionDeal->colInfoList);
                    if(actInfoFlag)//判断动作执行后是否采集信息：
                    {
                        timeState = collectInfo;
                        nextState = chkAction;//采集结束后，跳转到检测处理
                        TimeDelay1S=0;
                    }
                    else
                        timeState = chkAction;
                }
                else if(elapsed >= (actionDeal->timeDeal.wait))
                {/* 结束等待时，若检测时间不存在将默认进入等待结束，
                 * 若存在上述语句已经执行，因此直接进入actover操作*/
                    qDebug()<<"QTime.currentTime ="<<elapsed<<"ms";
                    if(!actionDeal->timeDeal.check)//无检测时间
                        timeState = waitover;
                    else
                    {
                        if(actionDeal->timeDeal.check>actionDeal->timeDeal.wait)//检测时间未在等待时间范围内，不进行检测，直接判断为测试通过
                            testResult =true;
                        timeState = actover;
                    }
                }
                tim_WaitLast = elapsed;
            }
            break;
        }
        case waitover:
        {
            colSize=ACT_Back;
            colInfoFlag=0x00;
            actInfoFlag = unitDeal->ActColInfo_Analy(ACT_Back,actionDeal->colInfoList);
            if(actInfoFlag)//判断动作执行后是否采集信息：
            {
                timeState = collectInfo;
                nextState = chkAction;
                TimeDelay1S=0;
            }
            else
                timeState = chkAction;
            break;
        }
        case pauseState:
            break;
        case actover://结束
            theActionChangedDeal(actionDeal->changedDeal);
            theActionOverTest(testResult);
            killTimer(timeID_T);
            break;
        }

        theActionPauseDeal();
    }
    else if(event->timerId() == timerProID)
    {
        timerProIDDeal();
    }
}

/*************************************************************
/函数功能：初始化动作执行参数
/函数参数：wu
/函数返回：wu
*************************************************************/
void Model_tAction::theActionInitParam()
{
    //初始化变量：
    testResult = false;     //:通常超时为测试失败,因此模式测试失败
    reChkCount=0;
    overtimeAct=0;
    IsFirstMemory=true;
    actIsRunning=false;
    TimeDelay1S=0;
    soundTimer=0;

    IsReRunning=0;

    colInfoDat.tempSoundInfo.clear();
    colInfoDat.Current=0;
    colInfoDat.volt=0;

    tim_WaitStart = QTime::currentTime();
    tim_WaitLast = 0;

    timeState = start;
}

/*************************************************************
/函数功能：启动动作执行
/函数参数：wu
/函数返回：wu
*************************************************************/
void Model_tAction::theActionStartDeal()
{
    //显示执行且保存到结果文件：
    ShowList <<"【"+ QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss")+"  INILoop:"+toStr(iniLoop+1)+"】 evaluateTheAction:"+actionDeal->actName;
    appendTheResultToFile("evaluateTheAction:"+actionDeal->actName);
    colSize = ACT_Front;
    colInfoFlag=0x00;
    actInfoFlag = unitDeal->ActColInfo_Analy(ACT_Front,actionDeal->colInfoList);
    //判断动作执行前是否采集信息：
    if(actInfoFlag)
    {
        timeState = collectInfo;
        nextState = exeAction;
    }
    else
        timeState = exeAction;
}

/*************************************************************
/函数功能：根据执行位置判断采集信息是否进行
/函数参数：当前位置
/函数返回：是否采集信息
/采集优先级顺序：电流--界面--图片--声音(依据开机参数能获取的顺序)
*************************************************************/
void Model_tAction::theActionCollectInfoDeal(bool curSize,uint16_t infoFlag)
{
    //采集电流：
    if((!(colInfoFlag & SIZE_Current))&&(infoFlag & SIZE_Current))
    {
        //处理的时间超过检测时间方可检测数据：
        if(actionDeal->timeDeal.wait > actionDeal->timeDeal.check)
        {
            for(int i=0;i<actionDeal->checkDeal.length();i++)
            {
                checkParam chkDeal = actionDeal->checkDeal.at(i);
                if(chkDeal.check == CHKCurrent)
                {
                    ShowList << tr("采集信息：电流");
                    colInfoDat.Current = Current;
                    if(rangeJudgeTheParam(chkDeal.range,chkDeal.min,chkDeal.max,colInfoDat.Current)==false)
                        return ;

                    colInfoFlag |= SIZE_Current;      //电流采集完成
                }
            }
        }
        else
            colInfoFlag |= SIZE_Current;      //电流采集完成
    }

    //采集电压：
    if((!(colInfoFlag & SIZE_Volt))&&(infoFlag & SIZE_Volt))
    {
        //处理的时间超过检测时间方可检测数据：
        if(actionDeal->timeDeal.wait > actionDeal->timeDeal.check)
        {
            for(int i=0;i<actionDeal->checkDeal.length();i++)
            {
                checkParam chkDeal = actionDeal->checkDeal.at(i);
                if(chkDeal.check == CHKVlot)
                {
                    ShowList << tr("采集信息：电压");
                    colInfoDat.volt = Volt;
                    if(rangeJudgeTheParam(chkDeal.range,chkDeal.min,chkDeal.max,colInfoDat.volt)==false)
                        return ;

                    colInfoFlag |= SIZE_Volt;      //电流采集完成
                }
            }
        }
        else
            colInfoFlag |= SIZE_Volt;      //电流采集完成
    }

    //采集界面：
    if((!(colInfoFlag & SIZE_Interface))&&(infoFlag & SIZE_Interface))
    {
        if((!isPRORunning)&&(proList.isEmpty()))
        {
            ShowList << tr("采集信息：界面");

            QString infoStr = unitDeal->ActColInfo_Read(curSize,"Interface",actionDeal->colInfoList);

            if(infoStr.isEmpty()==false)
                onProcessEXECmd(infoStr);
            else
                colInfoFlag|=SIZE_Interface;//未取到采集信息字符串，将退出采集
        }
        return ;
    }
    //采集图片：
    if((!(colInfoFlag & SIZE_Picture))&&(infoFlag & SIZE_Picture))
    {
        if((!isPRORunning)&&(proList.isEmpty()))
        {
            ShowList << tr("采集信息：图片");

            QString infoStr = unitDeal->ActColInfo_Read(curSize,"Picture",actionDeal->colInfoList);

            if(infoStr.isEmpty()==false)
                onProcessEXECmd(infoStr);
            else
                colInfoFlag|=SIZE_Picture;//未取到采集信息字符串，将退出采集
        }
        return ;
    }
    //采集声音：
    if((!(colInfoFlag & SIZE_Sound))&&(infoFlag & SIZE_Sound))
    {
        ShowList << tr("采集信息：声音");
        colInfoDat.tempSoundInfo.append(SoundV);
        if(++soundTimer >= ColSOUNDTimer)
        {
            //cout <<colInfoDat.tempSoundInfo.length();
            colInfoFlag |= SIZE_Sound;
        }
    }

    //信息采集完成,执行下一步：
    if(colInfoFlag == infoFlag)
        timeState=nextState;
}

/*************************************************************
/函数功能：动作执行处理
/函数参数：wu
/函数返回：wu
*************************************************************/
void Model_tAction::theActionExecuateDeal()
{
    if((!actIsRunning)&&(!overtimeAct))
    {
        ShowList << tr("~执行动作~")+actionDeal->actStr;
        if((actionDeal->actFlag == ACT_KEY)||(actionDeal->actFlag == ACT_BATVolt))
            startAction(actionDeal->actStr);//执行按键动作
        else if(actionDeal->actFlag == ACT_DELAYTime)
        {
            timeState = wait;
        }
        else
        {
            if((!isPRORunning)&&(proList.isEmpty()))
                onProcessEXECmd("Act:Script");
        }
        actIsRunning=true;
        TimeDelay1S=0;
    }
    else
    {
        /*等待动作处理完成时，会将标志位复位*/
        if(!actIsRunning)
        {
            if(actionDeal->timeDeal.wait)
            {
                ShowList << (tr("等待，时间：")+toStr(actionDeal->timeDeal.wait)+"mS");
                timeState = wait;
                tim_WaitStart = QTime::currentTime();
                tim_WaitLast = 0;
            }
            else
                timeState = waitover;
        }
        else
        {
            if(overtimeAct++ >= (actionDeal->timeDeal.end))//60000
            {
                ShowList << tr("Error:动作执行超时;");
                timeState = actover;//动作执行超时
            }
            else
            {
                //脚本运行连续执行
                if((actionDeal->actFlag==ACT_SCRIPT)&&(!isPRORunning)&&(proList.isEmpty()))
                {
                    if(TimeDelay1S % 1000 == 0)
                        onProcessEXECmd("Act:Script");
                    TimeDelay1S++;
                }
            }
        }
    }
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
            uint16_t curVoltage = actionDeal->actStr.mid(9).toUInt();
            if(cngDeal.dir)
            {
                //递增
                if((curVoltage)<cngDeal.max)
                {
                    if((curVoltage+cngDeal.step)<cngDeal.max)
                        curVoltage = curVoltage+cngDeal.step;
                    else
                        curVoltage = cngDeal.max;
                }
                else
                    curVoltage = cngDeal.min;
            }
            else
            {
                //递减
                if((curVoltage)>cngDeal.min)
                {
                    if((curVoltage-cngDeal.step)>cngDeal.min)
                        curVoltage = curVoltage-cngDeal.step;
                    else
                        curVoltage = cngDeal.min;
                }
                else
                    curVoltage = cngDeal.max;
            }
            actionDeal->actStr = "BAT:Volt:"+toStr(curVoltage);
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
    bool result = true;

    if(testChk.isEmpty())
    {
        //testResult = true;//当前无检测项，判断结果为真
        goto oncontinueDeal;
    }

    for(int i=0;i<testChk.length();i++)
    {
        switch(testChk.at(i).check)
        {
        case CHKCurrent:
        result &= unitDeal->chkCurrent(colInfoDat.Current,testChk.at(i));
        break;
        case CHKVlot:
        result &= unitDeal->chkVolt(colInfoDat.volt,testChk.at(i));
        break;
        case CHKSound:
        result &= unitDeal->chkSound(colInfoDat.tempSoundInfo,testChk.at(i));
        break;
        case CHKScript:
        result &= unitDeal->chkScript(savePath+"\\"+toStr(iniLoop)+"\\case.log",testChk.at(i));//any--检测脚本路径
        break;
        case CHKInterface:
        {
            QString infoStr = unitDeal->ActColInfo_Read(ACT_Back,"Interface",actionDeal->colInfoList);
            result &= unitDeal->chkInterface(infoStr,testChk.at(i));
            break;
        }
        case CHKRES:
        result &= unitDeal->chkRes(testChk.at(i));
        break;
        case CHKADBPIC:
        {
            QString infoStr = unitDeal->ActColInfo_Read(ACT_Back,"Picture",actionDeal->colInfoList);
            result &= unitDeal->chkADBPic(infoStr,testChk.at(i));
            break;
        }
        default:break;
        }
    }
oncontinueDeal:
    testResult = result;
    //是否继续等待:
    if((actionDeal->timeDeal.check)&&(actionDeal->timeDeal.check<actionDeal->timeDeal.wait))
    {
        ShowList<<tr("继续等待！");
        timeState = wait;
    }
    else
        timeState = actover;

    //检测失败处理:
    if(((testResult==false)&&(actionDeal->errorDeal == CHKERROR))
        ||((testResult)&&(actionDeal->errorDeal == CHKPASS)))
    {
        ShowList<<tr("Error:检测结果:")+toStr(testResult)+"   暂停测试";
        lastState = timeState;
        timeState = pauseState;
        PauseState = true;
    }

}

/*************************************************************
/函数功能：动作暂停处理
/函数参数：无
/函数返回：wu
*************************************************************/
void Model_tAction::theActionPauseDeal()
{
    if((PauseState)&&(timeState!=pauseState))
    {
        lastState = timeState;
        timeState = pauseState;
    }
    else if((!PauseState)&&(timeState==pauseState))
    {
        timeState = lastState;
    }
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
    timerProID = startTimer(1,Qt::PreciseTimer);
    isPRORunning=false;
    proItemString.clear();
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
void Model_tAction::onProcessEXECmd(QString cmdType)
{
    if(getDevNumber().isEmpty()==false)
        proList.append(ADBDevs);//先扫描设备，后运行命令
    else
    {
        if(NumberListIsSingle())
        {
            if(proItemString == "Act:Script")
            {
                IsOKCMDRunned=true;
                proList.append(actionDeal->actStr + " " +savePath.replace("/","\\")+"\\"+toStr(iniLoop)+" "+getDevNumber());//进程命令运行
            }
            else if(proItemString.contains("Interface"))
                proList.append(ACTIVITYFACE);
            else if(proItemString.contains("Picture"))
                proList.append(SCREENCAP);
        }
        else
        {
            if(proItemString == "Act:Script")
            {
                if(actIsRunning)
                {
                    actIsRunning=false;
                }
            }
            else if(proItemString.contains("Interface"))
                colInfoFlag |= SIZE_Interface;
            else if(proItemString.contains("Picture"))
                colInfoFlag |= SIZE_Picture;
        }

    }
    IsOKCMDRunned=false;
    proItemString = cmdType;
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
            if(proItemString == "Act:Script")
            {
                //any:Error:执行脚本时获取部分显示错误的信息，用于处理，以及后期显示执行按键的信息；
                if(String.indexOf("error: ") != -1) //含error字符串
                    IsOKCMDRunned=false;
            }
            else if(proItemString.contains("Interface"))
            {
                int idxOf = String.indexOf("launcher",0,Qt::CaseInsensitive);
                int ltidxOf = String.lastIndexOf("launcher",-1,Qt::CaseInsensitive);

                /*处理界面采集信息
                 * 1.包含界面字符串；
                 * 2.界面为Launcher，且前后位置存在不同：代表有2个及以上Launcher字符；
                 * 3.界面为非Launcher；
                */
                if((String.contains("mFocusedActivity: ActivityRecord"))&&( ((idxOf != -1) && (ltidxOf != idxOf)) || (idxOf == -1)))
                {
                    Model_String strDeal;
                    //QString faceStr = String;
                    //int startIndex=faceStr.indexOf("com.");
                    storageInfo_type_s infoStorage;

                    infoStorage.name = proItemString;
                    infoStorage.information = strDeal.StringDeal_Middle(String,"com.","}\r\r\n");//faceStr.mid(startIndex).remove("}\r\r\n");

                    unitDeal->ColInfo_Append(SIZE_Interface,infoStorage,actionDeal->checkDeal);

                    IsOKCMDRunned=true;
                }
            }
            else if(proItemString.contains("Picture"))
            {
                IsOKCMDRunned=true;
                //any：优化bug处理
                if(currentCMDString.contains("screencap"))
                {
                    if(String.contains("Error>>"))
                        IsOKCMDRunned=false;
                }
                else if(currentCMDString.contains("pull"))
                {
                    if(String.contains("adb:error:"))
                        IsOKCMDRunned=false;
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
                        if(proItemString == "Act:Script")
                        {
                            IsOKCMDRunned=true;
                            proList.append(actionDeal->actStr + " " +savePath.replace("/","\\")+"\\"+toStr(iniLoop)+" "+getDevNumber());//进程命令运行
                        }
                        else if(proItemString.contains("Interface"))
                            proList.append(ACTIVITYFACE_S(getDevNumber()));
                        else if(proItemString.contains("Picture"))
                            proList.append(SCREENCAP_S(getDevNumber()));

                        break;
                    }
                }
            }
        }
       /* else if(IsReRunning)
        {
            cout << IsReRunning;
            if(IsReRunning)
                onProcessEXECmd(proItemString);
            else
            {
                ShowList<<tr("Error:设备掉线。");
                lastState = start;//重新连接后，重新执行从动作开始时执行
                timeState = pauseState;
                PauseState = true;
            }
        }*/
        else
        {
            if(proItemString == "Act:Script")
            {
                //测试脚本运行：any:Error-脚本运行未结束时赋值，照成下一步的关执行失效
                if(IsOKCMDRunned)
                {
                    if(actIsRunning)
                    {
                        actIsRunning=false;
                    }
                }
            }
            else if(proItemString.contains("Interface"))
            {
                if(IsOKCMDRunned)
                    colInfoFlag |= SIZE_Interface;      //界面采集完成
            }
            else if((proItemString.contains("Picture"))&&(IsOKCMDRunned))
            {
                if(currentCMDString.contains("screencap"))
                {
                    QString picPath = savePath+"/"+actionDeal->actName;
                    QDir dir(picPath);
                    if(!dir.exists())
                    {
                        if(dir.mkpath(picPath) == false) //创建多级目录
                        {
                            cout << "创建属性文件夹失败！创建路径为："<<picPath;
                            return ;
                        }
                    }
                    picPath+="/"+QDateTime::currentDateTime().toString("yyyyMMddhhmmss")+".png";

                    if(getDevNumber().isEmpty())
                        proList.append(PULLFile(picPath));
                    else
                        proList.append(PULLFile_S(getDevNumber(),picPath));

                    //添加采集信息到临时变量
                    storageInfo_type_s infoStorage;
                    infoStorage.name = proItemString;
                    infoStorage.information = picPath.replace("/","\\");
                    unitDeal->ColInfo_Append(SIZE_Picture,infoStorage,actionDeal->checkDeal);
                    goto ToEndProcess;//跳转到进程结束处理，因为该多命令组合未完全执行结束，因此proCMD不清空
                }
                else if(currentCMDString.contains("pull"))
                {
                    colInfoFlag |= SIZE_Picture;      //界面采集完成
                }
            }
            //恢复状态
            proItemString.clear();
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






