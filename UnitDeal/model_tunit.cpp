#include "model_tunit.h"


/*
 * this is Unit Deal
 * any:增加结束单元测试函数
*/
Model_tUnit::Model_tUnit(tUnit *testUnit)
{
    dat_theUnit = *testUnit;
    sta_tUnitRunning = start;

    timeID_U = startTimer(100,Qt::PreciseTimer);
}

Model_tUnit::~Model_tUnit()
{
    if(isActionDeal)
    {
        theUnitResult(false);
        delete ActDeal;
    }
    killTimer(timeID_U);
}

/*************************************************************
/函数功能：定时器事件，处理测试
/函数参数：事件
/函数返回：无
*************************************************************/
void Model_tUnit::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == timeID_U)
    {
        switch (sta_tUnitRunning)
        {
        case start:
        {
            cnt_theUnitCycle = 0;
            isActionDeal=false;
            theUnitInitParam();
            break;
        }
        case dealAction:
        {
            curAction = dat_theUnit.actTest.at(inx_theActDat);
            ActDeal = new Model_tAction(cnt_theUnitCycle,&curAction);
            isActionDeal=true;

            connect(ActDeal,SIGNAL(theActionOverTest(bool)),this,SLOT(ontheActionResultSlot(bool)));
            sta_tUnitRunning = wait;
            break;
        }
        case overAction:
        {
            dat_theUnit.actTest.replace(inx_theActDat,curAction);
            delete ActDeal;
            isActionDeal=false;

            //动作判断
            inx_theActDat++;
            if(inx_theActDat < dat_theUnit.actTest.length())
                sta_tUnitRunning = dealAction;
            else
                sta_tUnitRunning = overUnit;
            break;
        }
        case overUnit:
        {
            //处理结果上传:主界面显示+测试报告生成
            bool exeResult = true;

            for(int i=0;i<arrayResult.length();i++)
            {
                //cout << arrayResult.at(i);
                if(!arrayResult.at(i))
                {
                    exeResult = false;
                    break;
                }
            }
            theUnitResult(exeResult);

            cnt_theUnitCycle++;
            //循环判断
            if(cnt_theUnitCycle < dat_theUnit.cycleCount)
                theUnitInitParam();
            else
                sta_tUnitRunning = over;
            break;
        }
        case over:
        {
            theUnitEnd();
            break;
        }
        case wait:
            break;
        }
    }
}

void Model_tUnit::theUnitInitParam()
{
    inx_theActDat = 0;
    arrayResult.clear();
    sta_tUnitRunning = dealAction;

    //清临时数据存储
    tempFaceInfo.clear();
    tempPicInfo.clear();

    theUnitStart();
}


/*************************************************************
/函数功能：结束动作测试结果
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tUnit::ontheActionResultSlot(bool result)
{
    arrayResult.append(result);
    sta_tUnitRunning = overAction;
    //cout << result;
}

/*************************************************************
/函数功能：获取unit循环次数
/函数参数：无
/函数返回：int
*************************************************************/
int Model_tUnit::getTheUnitLoop()
{
    return cnt_theUnitCycle;
}

