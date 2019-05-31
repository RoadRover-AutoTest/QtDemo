#include "model_tflow.h"

/* this is tFlow Deal
Model_tFlow::Model_tFlow()
{

    sta_tflowRunning = none;

    initProcessDeal();



}
*/

Model_tFlow::Model_tFlow(QList <tUnit> *testflow)
{
    dat_theTestFlow = testflow;
    sta_tflowRunning = start;
    //initProcessDeal();
    timer_theFlowID=startTimer(1000,Qt::PreciseTimer);//单元测试执行定时器：动作与动作直接执行可间隔1S
}


Model_tFlow::~Model_tFlow()
{
    //deleteProcessDeal();
}
/*************************************************************
/函数功能：定时器事件，处理测试
/函数参数：事件
/函数返回：无
//备注：启动测试后定时器中处理，1S循环     可在此加状态进行测试前的预处理
*************************************************************/
void Model_tFlow::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==timer_theFlowID)
    {
        switch(sta_tflowRunning)
        {
        case start:
        {
            cnt_theFlowCycle = 0;
            isUnitDeal=false;
            fixedInfo.clear();
            theFlowReturnUnitDeal();
            break;
        }
        case waitrecover:
        {
            if(!chkWorkState())
            {
                //每隔10S发送1次恢复工作状态指令
                if(reTime%10 == 0)
                {
                    ShowList << tr("evaluateTheFlow:恢复机器工作...");
                    keyClicked(AccKey+":on");
                    keyClicked(BatKey+":on");
                }
                //恢复工作失败，结束测试10S处理
                else if(reTime>10)
                    sta_tflowRunning = endflow;

                reTime++;
            }
            else
                sta_tflowRunning = lastState;
            break;
        }
        case dealunit:
        {
            if(unitIndex<dat_theTestFlow->length())
            {
                dat_theUnit = dat_theTestFlow->at(unitIndex);//此处不可使用局部变量，因进入Model_tUnit函数的是指针，局部变量函数运行结束会被释放，将找不到该指针
                unitDeal = new Model_tUnit(&dat_theUnit);
                isUnitDeal = true;

                connect(unitDeal,SIGNAL(theUnitStart()),this,SLOT(onStartTheUnitSlot()));
                connect(unitDeal,SIGNAL(theUnitEnd()),this,SLOT(onEndTheUnitSlot()));
                connect(unitDeal,SIGNAL(theUnitResult(bool)),this,SLOT(onUnitResultSlot(bool)));

                sta_tflowRunning = waitnull;
            }
            else
            {
                cout << "warn:unitIndex is more length.unitIndex:"+toStr(unitIndex);
            }
            break;
        }

        case unitover:
        {
            delete unitDeal;
            isUnitDeal = false;

            unitIndex++;
            if(unitIndex<dat_theTestFlow->length())
            {
                sta_tflowRunning = dealunit;
            }
            else
            {
                cnt_theFlowCycle++;
                if(cnt_theFlowCycle < WorkFrequency)
                    theFlowReturnUnitDeal();
                else
                    sta_tflowRunning = endflow;
            }
            break;
        }
        case over://any：不使用此状态，不在测试结束时进行检测并恢复机器
        {//any:Error:若设置的工作电流较大，关闭时不符合范围将无法关闭
            if(chkWorkState())
                sta_tflowRunning = endflow;
            else
            {
                lastState = endflow;
                sta_tflowRunning = waitrecover;
            }
            break;
        }
        case endflow:
        {
            flowEndTest();
            killTimer(timer_theFlowID);
            break;
        }
        case waitnull:break;
        default:break;
        }
    }
    else if(event->timerId()==timer_ProID)
    {
        //timerProIDDeal();
    }
}

/*************************************************************
/函数功能：检测机器正常工作状态：工作电流电流
/函数参数：无
/函数返回：无
//any:Error-恢复机器工作状态根据电流判断，若是电流符合，但机器处于掉电状态时将不会处理该恢复动作
*************************************************************/
bool Model_tFlow::chkWorkState()
{
    return (Current > WorkCurrent);
}

void Model_tFlow::theFlowReturnUnitDeal()
{
    unitIndex = 0;
    reTime = 0;

    if(chkWorkState())
        sta_tflowRunning = dealunit;
    else
    {
        lastState = dealunit;
        sta_tflowRunning = waitrecover;
    }
}

/*************************************************************
/函数功能：处理Unit开始测试槽函数：将该测试单元上传到主函数，用来显示
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tFlow::onStartTheUnitSlot()
{
    unitStartExe(dat_theUnit);
}

/*************************************************************
/函数功能：处理Unit测试结果槽函数：将该测试单元结果上传到主函数，用来显示
/函数参数：result
/函数返回：无
/备注：该测试结束代表Unit的单个测试结束，上传测试结果
*************************************************************/
void Model_tFlow::onUnitResultSlot(bool result)
{
    theUnitResult(result);
}

/*************************************************************
/函数功能：处理Unit结束测试槽函数：更新状态，进行下一项测试或结束测试
/函数参数：无
/函数返回：无
/备注：该测试结束代表Unit已经循环结束
*************************************************************/
void Model_tFlow::onEndTheUnitSlot()
{
    sta_tflowRunning = unitover;
}

/*************************************************************
/函数功能：结束测试
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tFlow::endTheTest()
{
    if(isUnitDeal)
    {
        delete unitDeal;
    }
    sta_tflowRunning = endflow;
}

/*************************************************************
/函数功能：获取外循环次数
/函数参数：无
/函数返回：int
*************************************************************/
int Model_tFlow::getTheFlowLoop()
{
    return cnt_theFlowCycle;
}

/*************************************************************
/函数功能：获取内循环次数
/函数参数：无
/函数返回：int
*************************************************************/
int Model_tFlow::getTheUnitLoop()
{
    return unitDeal->getTheUnitLoop();
}





#if 0

/*---------------------------------------this is Process option-----------------------------------------*/
/*************************************************************
/函数功能：初始化进程处理
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tFlow::initProcessDeal()
{
    mdl_PRODeal = new Model_Process;

    connect(mdl_PRODeal,SIGNAL(ProcessisOver(uint8_t)),this,SLOT(onProcessOverSlot(uint8_t)));
    connect(mdl_PRODeal,SIGNAL(ProcessOutDeal(int,QString)),this,SLOT(onProcessOutputSlot(int,QString)));

    mdl_PRODeal->ProcessPathJump(QCoreApplication::applicationDirPath());

    timer_ProID = startTimer(1,Qt::PreciseTimer);

    is_PRORunning=false;
    str_ProCMDDat.clear();
}

/*************************************************************
/函数功能：释放进程处理
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tFlow::deleteProcessDeal()
{
    killTimer(timer_ProID);
    delete mdl_PRODeal;
}

void Model_tFlow::proAppendSysCMD(QString str_ProCmd)
{
    lst_ProCMDDat.append(str_ProCmd);
}

/*************************************************************
/函数功能：进程执行处理
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tFlow::timerProIDDeal()
{
    if((lst_ProCMDDat.isEmpty()==false)&&(!proSysIsRunning()))
    {
        //cout << lst_ProCMDDat.length();
        mdl_PRODeal->ProcessStart(PROSYS,lst_ProCMDDat.first());

        str_ProCMDDat = lst_ProCMDDat.first();
        is_PRORunning=true;
        lst_ProCMDDat.removeFirst();
    }
}

/*************************************************************
/函数功能：进程输出槽函数处理
/函数参数：进程号  字符串
/函数返回：无
*************************************************************/
void Model_tFlow::onProcessOutputSlot(int pNum,QString String)
{
    //进程处理
    if((pNum==PROSYS)&&(String.isEmpty()==false))
    {
        if(String.contains("Error>>"))
            ShowList.append(String);
        else
        {
            if((!getTestRunState())&&(str_ProCMDDat == ADBDevs))
            {//处理设备扫描进程:显示设备列表
                QStringList devList = String.split("\r\n");

                if(devList.isEmpty()==false)
                    devList.removeFirst();

                if(devList.isEmpty()==false)
                {
                    for(int i=0;i<devList.length();)
                    {
                        QString tempString=devList.at(i);
                        if(tempString.contains("\tdevice"))
                        {
                            devList.replace(i,tempString.remove("\tdevice"));
                            if(devList.at(i).isEmpty())
                                devList.removeAt(i);
                            else
                                i++;
                        }
                        else
                            devList.removeAt(i);
                    }
                    ui->treeWidget->refreshDevNum1(devList);
                }
            }
            else
            {
                //测试进程输出处理
                testProcessOutputDeal(String);
                //cout<< String;
            }
        }
    }

}

/*************************************************************
/函数功能：进程结束处理
/函数参数：进程号
/函数返回：无
*************************************************************/
void Model_tFlow::onProcessOverSlot(uint8_t pNum)
{
    if(pNum==PROSYS)
    {
        //测试进程结束处理
        testProcessOverDeal();

        //恢复状态
        str_ProCMDDat.clear();
        is_PRORunning=false;
    }
}

/*************************************************************
/函数功能：停止系统进程执行logcat
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tFlow::proStopSysLogcat()
{
    mdl_PRODeal->stopProcess(getDevNumber(),"logcat");
}

/*************************************************************
/函数功能：停止系统进程执行uiautomator
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tFlow::proStopSysUiautomator()
{
    mdl_PRODeal->stopProcess(getDevNumber(),"uiautomator");
}

/*************************************************************
/函数功能：判断系统进程是否正在执行
/函数参数：无
/函数返回：无
/备注：判断的只是bat中某一条指令是否执行并不能判断bat执行是否正在运行
*************************************************************/
bool Model_tFlow::proSysIsRunning()
{
    if(mdl_PRODeal->GetProcessRunStatus(PROSYS) != noRun)
        return true;
    else
        return false;
}

#endif



















