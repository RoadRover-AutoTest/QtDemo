#ifndef MODEL_UART_H
#define MODEL_UART_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QTimerEvent>
#include <QDebug>

#include "model_include.h"

#define FRAME_MAX  256  //串口帧最大长度256

enum
{
    CMD_NEEDACK,                //0xAA
    CMD_NEEDNACK,               //0xBB
    CMD_ISACK                   //0x55
};

typedef enum
{
    Uart_Tx,
    Uart_Rx,
    Uart_Show
}uartDir;

enum
{
    Uart_Hex,
    Uart_NHex
};

typedef struct
{
    char cmd;
    char dat[FRAME_MAX];
    uint8_t len;
    bool ack;

}uartFrame;

class Model_UART:public QObject
{
    Q_OBJECT
public:
    Model_UART();
    ~Model_UART();



    bool Open(QString com,QString baud);                            //串口打开函数
    void Close();                                                   //串口关闭函数
    void p_connect();                                               //关联接收槽函数
    void p_disconnect();                                            //断开接收槽函数

    bool PortCompare(QString CurrentName);                          //判断当前端口是否存在
    QStringList PortList();                                         //查询端口列表

    void UartTxCmdDeal(char cmd,char* dat,char len,uint8_t ack);
    bool appendTxList(char cmd,char* dat,char len,uint8_t ack);

    bool isOpenCurrentUart();

    bool isUartAckOK();//提供最多3次发送是否响应的结果

private:
    QSerialPort *defSerial;

    QString openCom;

    int m_timerID;                              //定时器
    uint8_t RxCount;                            //接收时间计数，+1为10ms

    QByteArray revDatas;                        //接收数据
    QByteArray RxDealDats;                      //接收处理数据，串口解析
    QByteArray sendDatas;                       //发送数据

    bool isOpen;

    bool isAck;

    int m_timerIDACK;

    uint8_t reSendCount;//重复发送计数

    int timerCheckState; //检测串口在线状态

    QList <uartFrame> txList;      //将待发送的命令填充列表，然后在定时器中调用发送
    bool isWaitACK;
    int timerUartID;



    void PortSend(QByteArray g_SendDat);        //串口发送函数
    void UartRxCmdDeal(QByteArray Frame,uint8_t fLen);
    bool UartFrameCHK(QByteArray frame,uint8_t fLen);
    void timerUartIDDeal();

private slots:
    void ReadAllSlot();                         //接收槽函数

signals:
    void RxFrameDat(char cmd,uint8_t dLen,char *dat);               //接收帧数据信息
    void UartByteArrayBackStage(QByteArray Str,uartDir dir,bool IsHex);                    //传输过程数据传递给上层用作后台监控
    void UartRxAckResault(bool result);                             //响应结果 true:响应成功   false:0x5F响应失败 ,响应结果发出结束等待响应计时
    void UartError();

protected:
    void timerEvent(QTimerEvent *event);        //定时器处理函数
};

#endif // MODEL_UART_H
