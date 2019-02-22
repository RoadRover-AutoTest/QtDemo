#ifndef MODEL_PROCESS_H
#define MODEL_PROCESS_H

#include <QDebug>
#include <QProcess>
#include <QTimerEvent>
#include <QFileDialog>
#include <QTime>
#include "model_include.h"


#define P_Max 128



/*************************************************************
/运行状态
/noRun:无运行
/RunStart:开始运行
/Runing:正在运行
/RunOver:运行结束
*************************************************************/
typedef enum
{
    noRun,
    RunStart,
    Runing,
    RunOver
}sRun_e;

/*************************************************************
/进程结构体
/process:进程
/pNum:进程序号
/cmdList:命令执行列表
*************************************************************/
typedef struct
{
    QProcess *process;
    uint8_t pNum;
    QStringList cmdList;
}tProcess_t;

/*************************************************************
/进程参数结构体:进程列表参数不可更改，因此建立执行时参数结构体
/CmdRead:读命令执行信息
/CmdError:读命令执行错误信息
/isRun:运行状态
/rTime:运行计时
/pBarValue:运行进度条参数
*************************************************************/
typedef struct
{
    QString CmdRead;
    QString CmdError;
    sRun_e isRun;
    uint32_t rTime;
    uint8_t pBarValue;
}pParam_t;


class Model_Process:public QObject
{
    Q_OBJECT
public:
    Model_Process();
    ~Model_Process();

    //进程处理函数
    void Process_Add();
    void Process_Add(QString Workpath);
    void Process_Del(uint8_t pNum);

    //进程运行处理
    void ProcessStart(uint8_t pNum,QString cmdStr);
    void stopProcess(QString devNum,QString appStr);
    void ProcessPathJump(QString path);
    sRun_e GetProcessRunStatus(uint8_t pNum);
    bool GetProcessIsRun();


private:
    QList<tProcess_t > P_List;//进程列表
    pParam_t pParam[P_Max]; //进程参数

    int m_timerID1;//命令执行

    //内部进程执行
    void Process_Execute();

private slots:
    void ProcessReadAll();
    void ProcessReadError();
    void ProcessshowState(QProcess::ProcessState state);

protected:
    void timerEvent(QTimerEvent *event);

signals:  //信号
    void ProcessisOver(uint8_t pNum);       //进程执行完毕
    void ProcessBar(int Num,int value,QColor color);    //进程进度条
    void ProcessOutDeal(int pNum,QString String);
};




#endif // MODEL_PROCESS_H
