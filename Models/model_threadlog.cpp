#include "model_ThreadLog.h"


model_ThreadLog::model_ThreadLog(QObject *parent)
{
    proDeal =new Model_Process();
    stopped = false;

    connect(proDeal,SIGNAL(ProcessOutDeal(int,QString)),this,SLOT(StrShowDealSlot(int,QString)));
}
model_ThreadLog::~model_ThreadLog()
{
    delete proDeal;
}

void model_ThreadLog::stop()
{
    stopped=true;
}


void model_ThreadLog::run()
{
    //qreal i = 0;
    while (!stopped)//循环进入：当机器断开，脚本执行退出，再次连上自动从此处进入重新执行脚本打印log
    {
        //qDebug() << QString("in MyThread: %1").arg(i++);
        proDeal->ProcessStart(PROSYS,proCmdPath+" "+proCmdDev);
    }
    stopped = false;

}

void model_ThreadLog::SetRunPath(QString pathStr)
{
    proCmdPath=pathStr;
}

void model_ThreadLog::SetRunDEV(QString devStr)
{
    proCmdDev=devStr;
}

void model_ThreadLog::StrShowDealSlot(int num,QString str)
{
    if(num==PROSYS)
    {
        /*if(str.contains("[0] Error:"))
        {
            stopped=true;
            QMessageBox::warning(NULL, QString("log打印失败"), QString(str.mid(str.indexOf("Error:")+6)+"\n结束打印！"));
        }*/
        //StatusShow(str);
        cout << str;//采集信息，以便分析采集log执行状态
    }

}

