#include "model_uart.h"

Model_UART::Model_UART()
{

    RxCount=0;
    reSendCount=0;
    isWaitACK=0;

    isOpen=false;
    openCom.clear();

}

Model_UART::~Model_UART()
{
    if(isOpenCurrentUart())
    {
        Close();//关串口
    }
}

/*************************************************************
/函数功能：串口打开函数
/函数参数：com：端口   baud：波特率
/函数返回：bool：打开结果
*************************************************************/
bool Model_UART::Open(QString com,QString baud)
{
    qint32 portbaud;

    defSerial = new QSerialPort();

    if(baud == "4800")portbaud = 4800;
    else if(baud == "9600")portbaud = 9600;
    else if(baud == "14400")portbaud = 14400;
    else if(baud == "19200")portbaud = 19200;
    else if(baud == "38400")portbaud = 38400;
    else if(baud == "57600")portbaud = 57600;
    else if(baud == "115200")portbaud = 115200;
    else
    {
        portbaud = 115200;
    }

    defSerial->setPortName("\\\\.\\"+com.toLower());

    if(defSerial->open(QIODevice::ReadWrite) == false)
        return 0;

    //设置串口
    defSerial->setBaudRate(portbaud);
    //defSerial->setQueryMode(QSerialPort::);EventDriven
    defSerial->setFlowControl(QSerialPort::NoFlowControl);
    defSerial->setParity(QSerialPort::NoParity);
    defSerial->setDataBits(QSerialPort::Data8);
    defSerial->setStopBits( QSerialPort::OneStop);
    //defSerial->setTimeout(QSerialPort::TimeoutError);

    defSerial->close();//先关闭
    if(defSerial->open(QIODevice::ReadWrite) == false)//再打开，使配置生效
        return 0;

    p_connect();

    isOpen=true;
    openCom=com;

    m_timerIDACK=startTimer(CmdACKDelay,Qt::PreciseTimer);//CmdACKDelay未中断定时器，进入定时器处理再次发送，若中断代表在这段时间内已经接收到了响应数据
    m_timerID=startTimer(10,Qt::PreciseTimer);
    timerCheckState=startTimer(100,Qt::PreciseTimer);
    txList.clear();//结束即清空传输数据帧
    return 1;
}

/*************************************************************
/函数功能：串口关闭函数
/函数参数：无
/函数返回：无
*************************************************************/
void Model_UART::Close()
{
    if(isOpen)
    {
        killTimer(m_timerIDACK);
        killTimer(m_timerID);
        killTimer(timerCheckState);

        p_disconnect();

        defSerial->close();
        delete defSerial;

        isOpen=false;
        openCom.clear();
    }
}

/*************************************************************
/函数功能：判断当前串口是否打开
/函数参数：无
/函数返回：打开状态
*************************************************************/
bool Model_UART::isOpenCurrentUart()
{
    return isOpen;
}

/*************************************************************
/函数功能：查询端口列表
/函数参数：无
/函数返回：当前连接的串口列表
*************************************************************/
QStringList Model_UART::PortList()
{
    QStringList portNameList;

    //get name for choose
    QList<QSerialPortInfo>  infos = QSerialPortInfo::availablePorts();
    if(infos.isEmpty()==false)
    {
        foreach (QSerialPortInfo info, infos) {
            portNameList.append(info.portName());
        }
    }

    for(int i = 0;i < portNameList.length();i++)//把扫描到的串口号按数字从小到大排序
    {
        for(int j = i;j < portNameList.length();j++)
        {
            if(portNameList.at(i).mid(3,2).toInt() > portNameList.at(j).mid(3,2).toInt())
                portNameList.swap(i,j);
        }
    }
    return portNameList;
}

/*************************************************************
/函数功能：判断当前端口是否存在
/函数参数：CurrentName:当前端口名
/函数返回：bool：比较结果
*************************************************************/
bool Model_UART::PortCompare(QString CurrentName)
{
    QStringList portNameList;

    QList<QSerialPortInfo>  infos = QSerialPortInfo::availablePorts();
    if(infos.isEmpty()==false)
    {
        foreach (QSerialPortInfo info, infos) {
            portNameList.append(info.portName());
        }
    }

    for(int i = 0;i < infos.size();i++)
    {
        if(CurrentName == portNameList[i])
            return true;
    }
    return false;
}

/*************************************************************
/函数功能：关联接收槽函数
/函数参数：无
/函数返回：无
*************************************************************/
void Model_UART::p_connect()
{
    connect(defSerial,SIGNAL(readyRead()),this,SLOT(ReadAllSlot()));
}

/*************************************************************
/函数功能：断开接收槽函数
/函数参数：无
/函数返回：无
*************************************************************/
void Model_UART::p_disconnect()
{
    disconnect(defSerial,SIGNAL(readyRead()),this,SLOT(ReadAllSlot()));
}

/*************************************************************
/函数功能：定时处理函数
/函数参数：event：定时器事件
/函数返回：无
/备注：间隔50ms无数据接收，打包处理，数据处理分命令处理和字符串处理
/命令格式：0xFF 0xAA/0xBB/0x55 cmd len dat chk  以帧头区分是否是命令数据，若判断为命令数据，但数据长度等不对将丢弃处理
*************************************************************/
void Model_UART::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==m_timerID)
    {
        if(RxCount)
        {
            RxCount++;
            //50ms内无数据接收，将打包处理
            if(RxCount>5)
            {
                if(((uint8_t)revDatas[0]==0xFF)&&(((uint8_t)revDatas[1]==0xAA)||((uint8_t)revDatas[1]==0xBB)||((uint8_t)revDatas[1]==0x55)))
                    RxDealDats+=revDatas;//命令数据用来解析
                else
                    UartByteArrayBackStage(revDatas,Uart_Rx,Uart_NHex);//非命令数据，直接传输到子窗口
                revDatas.clear();
                RxCount=0;
            }
        }

        //串口数据解析，并将帧数据传输给子窗口
        if((RxDealDats.isEmpty()==false)&&((uint8_t)RxDealDats[0]==0xFF))
        {
            QByteArray frameArray;

            //qDebug()<<__FUNCTION__<<RxDealDats<<RxDealDats.length();
            if(((uint8_t)RxDealDats[1]==0x55)&&(RxDealDats.length()>=4))
            {
                frameArray=RxDealDats.left(4);

                UartRxCmdDeal(frameArray,frameArray.length());
                RxDealDats.remove(0,frameArray.length());
            }
            else if((((uint8_t)RxDealDats[1]==0xAA)||((uint8_t)RxDealDats[1]==0xBB))&&(RxDealDats.length()>=(5+(uint8_t)RxDealDats[3])))
            {
                frameArray=RxDealDats.left(5+(uint8_t)RxDealDats[3]);

                UartRxCmdDeal(frameArray,frameArray.length());

                //cout << frameArray << frameArray.length();
                RxDealDats.remove(0,frameArray.length());
            }
            else
            {
                RxDealDats.clear();
                cout << "ERROR：帧格式错误！长度错误或丢帧等";
            }
            //cout << "Rx:" << frameArray;
        }
        else //if((RxDealDats.isEmpty()==false)&&((uint8_t)RxDealDats[0]!=0xFF))
        {
            RxDealDats.clear();
            //cout << ("ERROR：帧格式错误！");
        }

    }
    else if(event->timerId()==m_timerIDACK)
    {

        //间隔CmdACKDelay时间，若无响应，重复发送
        if(reSendCount)
        {
            //超过设定次数将不再发送
            if(reSendCount>CmdReSendTimer)
            {
                reSendCount=0;
                isWaitACK=0;
                txList.clear();
                QMessageBox::warning(NULL, tr("警告"), tr("串口响应失败，请检查！"));
                UartError();
                return ;
            }
            else if(reSendCount>1)
                PortSend(sendDatas);
            reSendCount++;
        }
    }
    else if(event->timerId()==timerCheckState)
    {
        timerUartIDDeal();


        if(PortCompare(openCom) == false)
        {
            QMessageBox::warning(NULL,tr("警告"),tr("串口断开，请检查连接..."));
            Close();
            UartError();
        }
    }
}

/*************************************************************
/函数功能：接收槽函数
/函数参数：无
/函数返回：无
/备注：统一接收，定时器中统一处理数据
*************************************************************/
void Model_UART::ReadAllSlot()
{
    QByteArray tempRev;

    tempRev = defSerial->readAll();

    if(tempRev.isEmpty() == false)
    {
        RxCount=1;
        revDatas+=tempRev;
    }
}

/*************************************************************
/函数功能：串口发送函数
/函数参数：g_SendDat：发送数据
/函数返回：无
/备注：通过串口发送出去,长度由定义的命令决定
*************************************************************/
void Model_UART::PortSend(QByteArray g_SendDat)
{
    if(isOpenCurrentUart())
    {
        UartByteArrayBackStage(sendDatas,Uart_Tx,Uart_Hex);
        //cout << g_SendDat;

        defSerial->write(g_SendDat,g_SendDat.length());
    }
    else
    {
        cout <<"uart 发送异常-未打开";
    }
}

/*************************************************************
/函数功能：串口发送命令处理函数
/函数参数：cmd:命令  dat:数据  len:长度  ack：响应
/函数返回：无
*************************************************************/
void Model_UART::UartTxCmdDeal(char cmd,char* dat,char len,uint8_t ack)
{
    char commond[FRAME_MAX]={0};
    char check = 0;

    commond[0] = 0xFF;

    switch(ack)
    {
    case CMD_NEEDACK:
        commond[1]=0xAA;break;
    case CMD_NEEDNACK:
        commond[1] = 0xBB;break;
    case CMD_ISACK:
        commond[1] = 0x55;
        commond[2] = cmd;
        commond[3] = dat[0];//dat0传输校验结果
        len=4;
        goto SENDDat;
        break;
    default:return;
    }

    commond[2] = cmd;
    commond[3] = len;
    check ^= cmd;
    check ^= len;
    int i;
    for(i = 4;i < (4+len);i++)
    {
        commond[i] = *dat;
        check ^= (*dat);
        dat++;
    }
    commond[i] = check;
    len=len+5;

SENDDat:

    sendDatas = QByteArray(commond,len);
    PortSend(sendDatas);

    //需要响应的指令，设定重复发送
    if(ack==CMD_NEEDACK)
        reSendCount = 1;
    else
        reSendCount = 0;

}

/*************************************************************
/函数功能：串口接收槽函数
/函数参数：Frame:接收帧
/函数返回：无
*************************************************************/
void Model_UART::UartRxCmdDeal(QByteArray Frame,uint8_t fLen)
{
    //监控数据显示
    UartByteArrayBackStage(Frame,Uart_Rx,Uart_Hex);

    //解析处理
    if((uint8_t)Frame[1]!=0x55)
    {
        char chkResult=UartFrameCHK(Frame,fLen);//校验结果回传

        if(!chkResult)
        {
            chkResult=0x7F;
            UartTxCmdDeal(Frame[2],&chkResult,0,CMD_ISACK);
        }
        else
        {
            if((uint8_t)Frame[1]==0xAA)
            {
                chkResult=0x01;
                UartTxCmdDeal(Frame[2],&chkResult,0,CMD_ISACK);
            }
            char dat[BUFSIZ] = {0};
            for(int i=0;i<Frame[3];i++)
            {
                dat[i]=Frame[4+i];
            }
            RxFrameDat(Frame[2],Frame[3],dat);//将数据传递给上层应用
        }
    }
    else
    {
        reSendCount=0;
        isWaitACK=0;
        //处理0x55响应
        UartRxAckResault((uint8_t)Frame[3]);
    }


}

/*************************************************************
/函数功能：校验帧数据
/函数参数：frame:帧数据 fLen:帧长度
/函数返回：bool：校验结果
*************************************************************/
bool Model_UART::UartFrameCHK(QByteArray frame,uint8_t fLen)
{
    uint8_t check=0;
    QString debugStr;

    for(int i = 0;i <(2+frame[3]) ;i++)
    {
        check ^= frame[i+2];//从cmd~dat计算校验值
    }

    if(check != (uint8_t)frame[fLen-1])
    {
        debugStr="ERROR:校验和错误，应为"+QString::number(check);
       // UartByteArrayBackStage(debugStr,Uart_Show,Uart_NHex);//any:显示警告
        return false;
    }
    else
        return true;
}

/*************************************************************
/函数功能：填充串口发送列表
/函数参数：同命令发送
/函数返回：无
/备注：为避免堵塞串口，将设定的串口数据先以链表形式存储，之后一个个发送
*************************************************************/
bool Model_UART::appendTxList(char cmd,char* dat,char len,uint8_t ack)
{
    bool status=isOpenCurrentUart();
    if(status)
    {
        uartFrame uartDat;

        uartDat.cmd = cmd;
        for(int i=0;i<len;i++)
        {
            uartDat.dat[i] = dat[i];
        }

        uartDat.len = len;
        uartDat.ack = ack;
        txList.append(uartDat);
    }
    else
    {
        QMessageBox::warning(NULL, tr("警告"), tr("未打开串口，请处理！"));
    }
    return status;
}

/*************************************************************
/函数功能：处理定时串口ID数据
/函数参数：无
/函数返回：无
*************************************************************/
void Model_UART::timerUartIDDeal()
{
    if((txList.isEmpty() == false) && (!isWaitACK))
    {
        uartFrame uartDat=txList.at(0);

        if(uartDat.ack == CMD_NEEDACK)
        {
            isWaitACK=1;
        }

        cout << "txList:" <<txList.length();
        UartTxCmdDeal(uartDat.cmd,uartDat.dat,uartDat.len,uartDat.ack);
        txList.removeFirst();
    }
}

