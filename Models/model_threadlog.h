#ifndef MODEL_THREADLOG_H
#define MODEL_THREADLOG_H


#include <QThread>
#include <QMessageBox>
//#include "app_process.h"
#include "model_include.h"
#include "model_process.h"

class model_ThreadLog : public QThread
{
    Q_OBJECT


public:
    model_ThreadLog(QObject *parent = 0);
    ~model_ThreadLog();

    void stop();
    void SetRunPath(QString pathStr);
    void SetRunDEV(QString devStr);

public:
    void run();
private:
    volatile bool stopped;

    Model_Process *proDeal;

    QString proCmdPath;
    QString proCmdDev;

private slots:
    void StrShowDealSlot(int num,QString str);
signals:
    void StatusShow(QString str);
};








#endif // MODEL_THREADLOG_H
