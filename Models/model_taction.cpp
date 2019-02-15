#include "model_taction.h"


/* this is Action Deal
 * ：测试流程改为：关ACC--开ACC--脚本（优点，测试单元执行结束机器仍为正常工作状态，无需恢复；测试记忆功能也可在一个测试单元中处理）
 * 增加任务：
 * 1.关机或ACC时读取机器界面-用来判断记忆功能；
 * 2.测试变动；
 * 3.BAT测试增加改变电压的测试；
 * 4.动作执行完成处理
*/
Model_tAction::Model_tAction(int loop,tAction *Action)
{
    iniLoop = loop;
    actionDeal = Action;
    timeState = start;
    timeID_T = startTimer(1);//开启定时器
}

Model_tAction::~Model_tAction()
{

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
            testResult = false;     //:通常超时为测试失败,因此模式测试失败
            timeCount=0;
            reChkCount=0;
            overtimeAct=0;
            IsFirstMemory=true;

            ShowList << "evaluateTheAction:"+actionDeal->actName;
            appendTheResultToFile("evaluateTheAction:"+actionDeal->actName);

            testString.clear();
            testString = actionDeal->actStr;

            actIsRunning=true;
            timeState = waitnull;   //等待动作执行完成
            break;
        }
        case waitnull:
        {
            //空等-等待动作处理完成
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
                //动作执行超时处理：
                if(overtimeAct++ > 60000)
                    timeState = endover;
            }
            break;
        }
        case wait:
        {
            //计数并判断
            timeCount++;

            if(timeCount == actionDeal->timeDeal.wait)
            {
                if(IsDealCheck())
                    timeState = waitover;
                else
                {
                    testResult =true;
                    timeState = endover;
                }
            }
            else if(timeCount == actionDeal->timeDeal.check)
                timeState = checkover;
            break;
        }
        case waitover:
        {
            //1S监测一次结果
            if(reChkCount%5000 == 0)
            {
                //结果处理完成  或  判断结束后重复检查超时
                if((theActionResultDeal(actionDeal->checkDeal)==false)||((reChkCount >= actionDeal->timeDeal.end)))
                {
                    testString.clear();//在结果处理时有对记忆进行赋值，若开始判断结果时将清除数据，避免对后续操作影响
                    theActionCheckReault(actionDeal->checkDeal);//检测
                    timeState = endover;
                }
            }
            reChkCount++;
            break;
        }
        case checkover:
        {
            //检测并继续测试
            if(theActionResultDeal(actionDeal->checkDeal)==false)
            {
                theActionCheckReault(actionDeal->checkDeal);
                timeState = wait;
            }
            else
                timeState = wait;
            break;
        }
        case endover:
        {//结束
            theActionChangedDeal(actionDeal->changedDeal);
            theActionOverTest(testResult);
            killTimer(timeID_T);
            break;
        }
        }
    }
}

/*************************************************************
/函数功能：设定的check时间小于等待时间时可检测，大于时证明达不到检测条件
/函数参数：无
/函数返回：结果
*************************************************************/
bool Model_tAction::IsDealCheck()
{
    return (!actionDeal->timeDeal.check);//((actionDeal->timeDeal.check <= actionDeal->timeDeal.wait) &&
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
/函数功能：结果预处理：主要针对开机检测：先判断电流值是否达到，再判断界面是否打开
/函数参数：检测列表
/函数返回：wu
*************************************************************/
bool Model_tAction::theActionResultDeal(QList <checkParam> testChk)
{
    bool isDeal = false;
    bool result =false;

    for(int i=0;i<testChk.length();i++)
    {
        if(testChk.at(i).check == CHKCurrent)
        {
            result = rangeJudgeTheParam(testChk.at(i).range,testChk.at(i).min,testChk.at(i).max,Current);
            if(result==false)
            {
                isDeal=true;
                break;
            }
        }
        else if(testChk.at(i).check == CHKMEMORY)
        {
            if(onFace.isEmpty())
            {
                if(IsFirstMemory)
                {
                    IsFirstMemory=false;
                    testString = "checkMemory";
                }
                isDeal=true;
            }
            break;
        }
    }
    return isDeal;
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
        case CHKMEMORY:
        result &= chkMemory(testChk.at(i));
        break;
        case CHKRES:
        result &= chkRes(testChk.at(i));
        break;
        case CHKADBPIC:
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
bool Model_tAction::chkMemory(checkParam memory)
{
    ShowList<< "checkTheAction:检测记忆..."+onFace;
    bool result = false;

    if(memory.isMemory)
    {
        if((onFace.isEmpty()==false)&&(onFace == offFace))
            result = true;
    }
    else
    {
        if(onFace.isEmpty() == false)
            result = true;
    }

    appendTheResultToFile("Judge:Memory:"+offFace);
    appendTheResultToFile("Check:Memory:"+onFace);
    appendTheResultToFile("Result:Memory:"+toStr(result));

    return result;
}

/*************************************************************
/函数功能：检测，，
/函数参数：动作定义
/函数返回：wu
*************************************************************/
bool Model_tAction::chkRes(checkParam res)
{
    return false;
}







