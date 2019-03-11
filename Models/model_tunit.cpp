#include "model_tunit.h"


/*
 * this is Unit Deal
 * any:增加结束单元测试函数
*/
Model_tUnit::Model_tUnit(tUnit *testUnit)
{
    UnitDeal = *testUnit;
    testState = start;

    timeID_U = startTimer(100);
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
        switch (testState)
        {
        case start:
        {
            cycleCount = 0;
            isActionDeal=false;
            testState = initUnit;
            break;
        }
        case initUnit:
        {
            actIndex = 0;
            arrayResult.clear();
            testState = dealAction;

            //清临时数据存储
            tempFaceInfo.clear();
            tempPicInfo.clear();

            theUnitStart();
            break;
        }
        case dealAction:
        {
            curAction = UnitDeal.actTest.at(actIndex);
            ActDeal = new Model_tAction(cycleCount,&curAction);
            isActionDeal=true;

            connect(ActDeal,SIGNAL(theActionOverTest(bool)),this,SLOT(ontheActionResultSlot(bool)));
            testState = wait;
            break;
        }
        case overAction:
        {
            UnitDeal.actTest.replace(actIndex,curAction);
            delete ActDeal;
            isActionDeal=false;

            //动作判断
            actIndex++;
            if(actIndex < UnitDeal.actTest.length())
                testState = dealAction;
            else
                testState = overUnit;
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

            cycleCount++;
            //循环判断
            if(cycleCount < UnitDeal.cycleCount)
                testState = initUnit;
            else
                testState = over;            
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


/*************************************************************
/函数功能：结束动作测试结果
/函数参数：无
/函数返回：无
*************************************************************/
void Model_tUnit::ontheActionResultSlot(bool result)
{
    arrayResult.append(result);
    testState = overAction;
    //cout << result;
}

/*************************************************************
/函数功能：获取unit循环次数
/函数参数：无
/函数返回：int
*************************************************************/
int Model_tUnit::getTheUnitLoop()
{
    return cycleCount;
}

