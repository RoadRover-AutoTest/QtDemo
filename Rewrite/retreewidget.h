#ifndef RETREEWIDGET_H
#define RETREEWIDGET_H

#include <QObject>
#include <QTreeWidget>
#include <QComboBox>
#include <QPushButton>
#include <QPainter>
#include <QApplication>
#include <QStyleOptionFrameV3>

#include "Models/model_include.h"
//#include "Models/model_winapi.h"    //将USB口的刷新放置在模块里
#include "Models/model_process.h"

typedef enum
{
    colItem=1,                      //项目内容处在第一列

    topUart=0,                        //顶层目录
    //topCAN,
    topDEV,

    uartCOM=0,                      //串口目录
    uartBAUD,
    uartUSE,

    //canType=0,                      //CAN设备
    //canBaud,
    //canUSE,

    //devWIFI=0,                    //设备连接
    //devUSB,
    //devNum,
    devNum1=0,
    devUSE,

    //devIP=0,
    //devCONNECT,

}treeItem;



class reTreeWidget:public QTreeWidget
{
    Q_OBJECT
public:
    reTreeWidget(QWidget *parent);
    ~reTreeWidget();


    void interfaceTreeWidgetInit();
    void refreshUartCOM(QStringList comList);
    void refreshDevNum1(QStringList devList);


    void setCheckedState(int topItem,bool checked);
    QComboBox *getDevNumberComboBox();


private:
    bool isSetChecked;  //当启用在主函数调用不合适时，为其设置为原状态，但此状态不要在次检测tree改变内容，避免再次调用

    //Model_WinApi *WinDeal;
    //QList<tDevice_t> DevsList;


    QString getQComboBoxCurrentTest(QTreeWidgetItem *curItem);

    //void initWinAPIDeal();
    //void deleteWinAPIDeal();




private slots:

    //void wifiConnectDeal(bool checked);


    void itemChangedCheckedState(QTreeWidgetItem *curItem ,int column);

    //void refreshUSBPort();
    //void clickedUSBPort(int index);


signals:
    void uartOpen(QString com,QString baud);
    void uartClose();

    //void canOpen(QString type,QString baud);
    //void canClose();

    void devUseState(bool isUse);


    //void wifiIsConnect(bool isCNT,QString ip);

protected:
    void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // RETREEWIDGET_H
