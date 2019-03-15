#include "resupanddownloads.h"
#include "ui_resupanddownloads.h"

ResUpAndDownLoads::ResUpAndDownLoads(bool dir,QList <keyControl> *keyInfo,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResUpAndDownLoads)
{
    ui->setupUi(this);

    isDown = dir;

    keyInfoList=keyInfo;

    ui->stackedWidget->setCurrentIndex(isDown);

    keyUart=new Model_UART;

    ui->comboBox_COM->addItems(keyUart->PortList());//可用端口列表

    isAck=false;

    timeIDSendUart=startTimer(CmdACKDelay);//CmdACKDelay处理一次串口发送或接收

    isStartUartTx=false;

}

ResUpAndDownLoads::~ResUpAndDownLoads()
{
    killTimer(timeIDSendUart);

    delete keyUart;
    delete ui;
}

/*************************************************************
/函数功能：设置下载项目名
/函数参数：name
/函数返回：wu
*************************************************************/
void ResUpAndDownLoads::setdownItemName(QString name)
{
        ui->lineEdit_OUTItem->setText(name);
}

/*************************************************************
/函数功能：定时处理函数
/函数参数：event：定时器事件
/函数返回：无
*************************************************************/
void ResUpAndDownLoads::timerEvent(QTimerEvent *event)
{
    if(event->timerId()==timeIDSendUart)
    {
        if(isStartUartTx)
        {
            if(isDown)
            {
                downUartDeal();
            }
            else
            {
                upUartDeal();
            }
        }

    }
}

/*************************************************************
/函数功能：串口下载处理
/函数参数：无
/函数返回：无
*************************************************************/
void ResUpAndDownLoads::downUartDeal()
{
    //cout << keyIndex;
    if(keyIndex < (MaxKey+1))//传递项目号 + 按键信息
    {
        if((isAck) || (!keyIndex))
        {
            char buf[BUFSIZ]={0};//不可用指针，当数据较多时无法正确赋值，因未分配内存，照成数据混乱
            int len = 0;

            QByteArray arrayBuf ;

            if(!keyIndex)
            {
                //下载项目号
                arrayBuf = ui->lineEdit_OUTItem->text().toLatin1();

                len = arrayBuf.length();
                strcpy(buf,arrayBuf);

                ui->label_Show->setText("Down:item "+arrayBuf);
                ui->progressBar->setValue(ui->progressBar->value()+proValueAdd);
            }
            else
            {
                //下载按键信息
                if(keyInfoList->length()>(keyIndex-1))
                {
                    Model_String strDeal;
                    keyControl keyInfo = keyInfoList->at(keyIndex-1);

                    arrayBuf = keyInfo.name.toLatin1();

                    len = arrayBuf.length();
                    strcpy(buf,arrayBuf);

                    buf[len++] = '*';
                    buf[len++]=keyInfo.isUse;
                    buf[len++]=keyInfo.type;


                    if((keyInfo.type == Can1_1) || (keyInfo.type == Can1_2))
                    {
                        QString idStr;
                        arrayBuf.clear();

                        //数据转换时奇数个将在后面补0，因此提前处理
                        if(keyInfo.CANID.length()%2)
                            idStr='0'+keyInfo.CANID;
                        else
                            idStr=keyInfo.CANID;
                        strDeal.StringToHex(idStr,arrayBuf);
                        for(int i=0;i<arrayBuf.length();i++)
                        {
                            buf[len++] = arrayBuf[i];
                        }

                        arrayBuf.clear();

                        strDeal.StringToHex(keyInfo.CANDat1,arrayBuf);
                        for(int i=0;i<arrayBuf.length();i++)
                        {
                            buf[len++] = arrayBuf[i];
                        }

                        if(keyInfo.type == Can1_2)
                        {
                            arrayBuf.clear();

                            strDeal.StringToHex(keyInfo.CANDat2,arrayBuf);
                            for(int i=0;i<arrayBuf.length();i++)
                            {
                                buf[len++] = arrayBuf[i];
                            }
                        }
                    }
                    ui->label_Show->setText("Down:key"+QString::number(keyIndex-1)+"  "+keyInfo.name);
                    ui->progressBar->setValue(ui->progressBar->value()+proValueAdd);
                }
            }

            keyUart->UartTxCmdDeal(CMDItemWrite+keyIndex,buf,len,CMD_NEEDACK);
            isAck=false;
            txCount=0;
            keyIndex++;
        }
        else
        {
            //响应延时3倍时间等待，若仍无响应终止传输:在200ms内，串口函数中将再次发送
            if((++txCount) > (CmdReSendTimer+2))
            {
                cout << ("串口传输无响应，请检查！");
                QMessageBox::warning(NULL, tr("提示"), tr("串口传输无响应，请检查！"));
                txCount=0;
                keyIndex=0;
                this->close();
            }
        }
    }
    else
    {
        //cout << keyIndex;
        keyIndex=0;
        ui->progressBar->setValue(100);
        this->close();
    }
}

/*************************************************************
/函数功能：串口上传处理
/函数参数：无
/函数返回：无
*************************************************************/
void ResUpAndDownLoads::upUartDeal()
{
    cout ;
    if(!rxCount)
    {
        char read=0;
        keyUart->UartTxCmdDeal(CMDItemRead,&read,1,CMD_NEEDNACK);
    }
    else
    {
        //长时间未接收到数据
        if(rxCount>30)
        {
            QMessageBox::warning(NULL, tr("提示"), tr("该小板无项目按键信息！"));
            rxCount=0;

            this->close();
        }
    }
    rxCount++;
}

bool ResUpAndDownLoads::downIsFileSave()
{
    return (ui->radioDownAndFile->isChecked() || ui->radioOnlyFileBtn->isChecked());
}

bool ResUpAndDownLoads::downIsUartHardware()
{
    return (ui->radioDownAndFile->isChecked() || ui->radioOnlyHardBtn->isChecked());
}

/*************************************************************
/函数功能：点击下载
/函数参数：无
/函数返回：无
*************************************************************/
void ResUpAndDownLoads::on_pushButton_OUT_clicked()
{
    QString itemName = ui->lineEdit_OUTItem->text();

    if(itemName.isEmpty())
    {
        QMessageBox::warning(NULL, tr("提示"), tr("项目名不可为空，请填写！"));
        return ;
    }

    if((keyUart->isOpenCurrentUart()==false)&&(downIsUartHardware()))
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        return ;
    }

    ui->pushButton_OUT->setEnabled(false);

    //SAVE To File
    //if(ui->checkBox_Save->checkState()==Qt::Checked)
    if(downIsFileSave())
    {
        ui->label_Show->setText("Down:SAVE KEY To KeyInfo.xml File. ");
        Model_XMLFile xmlSave;
        QStringList keylist;

        bool isAppend=true;

        xmlSave.createKeyInfoXML();

        if(xmlSave.hasItemKeyInfomation(itemName))
        {
            if(QMessageBox::information(NULL, tr("提示"), tr("文件中该项目已存在，是否替换按键定义？？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)
            {
                goto sendTOTestHard;
            }
            isAppend=false;
        }

        for(int i=0;i<keyInfoList->length();i++)
        {
            keyControl keyInfo = keyInfoList->at(i);
            keylist.clear();

            keylist << "KEY"+QString::number(i+1) << keyInfo.name << QString::number(keyInfo.isUse) << keyInfo.des << QString::number(keyInfo.type) << keyInfo.CANID << keyInfo.CANDat1 << keyInfo.CANDat2;

            xmlSave.appendKeyInfoXML(itemName,isAppend,keylist);
        }
    }

    ui->progressBar->setValue(ui->progressBar->value()+proValueAdd);

sendTOTestHard:
    keyIndex=0;
    if(downIsUartHardware())
        isStartUartTx=true;
    else
    {
        ui->progressBar->setValue(100);
        this->close();
    }
}

/*************************************************************
/函数功能：点击上传
/函数参数：无
/函数返回：无
*************************************************************/
void ResUpAndDownLoads::on_pushButton_IN_clicked()
{
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行上传操作！"));
        return ;
    }

    isStartUartTx = true;
    rxCount=0;
}

/*************************************************************
/函数功能：获取当前处理项目
/函数参数：wu
/函数返回：无
*************************************************************/
QString ResUpAndDownLoads::getCurrentItem()
{
    if(isDown)
    {
        return ui->lineEdit_OUTItem->text();
    }

    return ui->label_inItem->text();
}

/*************************************************************
/函数功能：串口接收处理函数
/函数参数：
/函数返回：无
*************************************************************/
void ResUpAndDownLoads::UartRxDealSlot(char cmd,uint8_t dLen,char *dat)
{
    if(cmd == CMDItemRead)
    {
        QString item(dat);
        cout << item << dLen;

        ui->label_inItem->setText(item);

        rxCount=0;

        this->close();
    }
}

/*************************************************************
/函数功能：接收串口响应处理槽函数
/函数参数：ack：响应结果
/函数返回：无
*************************************************************/
void ResUpAndDownLoads::UartRxAckResault(bool ack)
{
    if(!ack)
        cout << "Frame ack is " << ack;

    isAck=true;
}

/*************************************************************
/函数功能：打开或关闭串口
/函数参数：checked  true :开  false:关
/函数返回：无
*************************************************************/
void ResUpAndDownLoads::on_pushButton_UART_clicked(bool checked)
{
    if(checked)
    {
        keyUart->Close();//关串口

        ui->pushButton_UART->setText(tr("打开串口"));
    }
    else
    {
        if(!keyUart->Open(ui->comboBox_COM->currentText(),ui->comboBox_BAUD->currentText()))
        {
            return ;
        }
        else
        {
            connect(keyUart,SIGNAL(RxFrameDat(char,uint8_t,char*)),this,SLOT(UartRxDealSlot(char,uint8_t,char*)));
            connect(keyUart,SIGNAL(UartRxAckResault(bool)),this,SLOT(UartRxAckResault(bool)));
        }


        ui->pushButton_UART->setText(tr("关闭串口"));
    }
}
