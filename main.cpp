#include "mainwindow.h"
#include "dialoglogin.h"
#include <QApplication>
#include <QtMsgHandler>
#include <QTranslator>

//*************************************************************************************************
//ATtool:致力于自动化测试
//集成进程，线程，串口模块，opencv，等操作；
//注意事项：
//  1.编译时需将opencv相应的库放置在编译目录下，否则会提示动态库找不到（放置在环境目录下无效---待解释）；
//  2.
//作者：李书会
//日期：20190425更新
//*************************************************************************************************


void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString txtMessage;
    Q_UNUSED(context);
    switch(type)
    {
        case QtDebugMsg:    //调试信息提示
            txtMessage = QString("Debug: %1").arg(msg);
            break;

        case QtWarningMsg:    //一般的warning提示
            txtMessage = QString("Warning: %1").arg(msg);
            break;

        case QtCriticalMsg:    //严重错误提示
            txtMessage = QString("Critical: %1").arg(msg);
            break;

        case QtFatalMsg:    //致命错误提示
            txtMessage = QString("Fatal: %1").arg(msg);
            abort();
            break;
    default:break;
    }

    //保存输出相关信息到指定文件
    QFile outputFile("customMessageLog.txt");
    outputFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream textStream(&outputFile);
    textStream << txtMessage << endl;
    outputFile.close();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qInstallMessageHandler(customMessageHandler); //注册MsgHandler回调函数

    QTranslator translator;
    if(translator.load("uav_tr_laEN"))
    {
        qApp->installTranslator(&translator);
    }


    DialogLogin login_w(&translator);

    if(login_w.exec()==QDialog::Accepted)
    {
        MainWindow w;
        w.show();
        return a.exec();
    }
    else
        return 0;
}
