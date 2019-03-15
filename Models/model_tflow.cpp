#include "model_tflow.h"

/* this is tFlow Deal*/
Model_tFlow::Model_tFlow(QList <tUnit> *testflow)
{
    theFlow = testflow;

    flowState = start;
    timeID_UF=startTimer(1000);//单元测试执行定时器：动作与动作直接执行可间隔1S
}

Model_tFlow::~Model_tFlow()
{

}
/*************************************************************
/函数功能：定时器事件，处理测试
/函数参数：事件
/函数返回：无
//备注：启动测试后定时器中处理，1S循环     可在此加状态进行测试前的预处理
*************************************************************/
void Model_tFlow::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==timeID_UF)
    {
        switch(flowState)
        {
        case start:
        {
            unitIndex = 0;
            cycleCount = 0;
            reTime = 0;
            isUnitDeal=false;
            fixedInfo.clear();


            if(chkWorkState())
                flowState = dealunit;
            else
            {
                lastState = dealunit;
                flowState = waitrecover;
            }

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
                //恢复工作失败，结束测试
                else if(reTime>60)
                    flowState = endflow;

                reTime++;
            }
            else
                flowState = lastState;
            break;
        }
        case dealunit:
        {
            if(unitIndex<theFlow->length())
            {
                curUnit = theFlow->at(unitIndex);//此处不可使用局部变量，因进入Model_tUnit函数的是指针，局部变量函数运行结束会被释放，将找不到该指针
                unitDeal = new Model_tUnit(&curUnit);
                isUnitDeal = true;

                connect(unitDeal,SIGNAL(theUnitStart()),this,SLOT(onStartTheUnitSlot()));
                connect(unitDeal,SIGNAL(theUnitEnd()),this,SLOT(onEndTheUnitSlot()));
                connect(unitDeal,SIGNAL(theUnitResult(bool)),this,SLOT(onUnitResultSlot(bool)));

                flowState = waitnull;
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
            if(unitIndex<theFlow->length())
            {
                flowState = dealunit;
            }
            else
            {
                cycleCount++;
                if(cycleCount < WorkFrequency)
                {
                    unitIndex = 0;
                    reTime = 0;

                    if(chkWorkState())
                        flowState = dealunit;
                    else
                    {
                        lastState = dealunit;
                        flowState = waitrecover;
                    }
                }
                else
                    flowState = over;
            }
            break;
        }
        case over:
        {
            if(chkWorkState())
                flowState = endflow;
            else
            {
                lastState = endflow;
                flowState = waitrecover;
            }
            break;
        }
        case endflow:
        {
            flowEndTest();
            killTimer(timeID_UF);
            break;
        }
        case waitnull:break;

        }
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

/*************************************************************
/函数功能：处理Unit开始测试槽函数：将该测试单元上传到主函数，用来显示
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tFlow::onStartTheUnitSlot()
{
    unitStartExe(curUnit);
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
    flowState = unitover;
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
    flowState = over;//any:Error:若设置的工作电流较大，关闭时不符合范围将无法关闭
}

/*************************************************************
/函数功能：获取外循环次数
/函数参数：无
/函数返回：int
*************************************************************/
int Model_tFlow::getTheFlowLoop()
{
    return cycleCount;
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

