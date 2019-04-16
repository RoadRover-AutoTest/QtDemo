#include "reshardware.h"
#include "ui_reshardware.h"

ResHardware::ResHardware(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResHardware)
{
    ui->setupUi(this);

    refreshitemName(WorkItem);

    connect(ui->comboBox_itemName,SIGNAL(activated(QString)),this,SLOT(itemNameSlot(QString)));
    connect(ui->comboBox_itemName,SIGNAL(currentTextChanged(QString)),this,SLOT(itemNameSlot(QString)));


    keyList.clear();

    tableWidgetInit();

    if(ui->comboBox_itemName->currentText().isEmpty() == false)
    {
        readItemKeyInfo(ui->comboBox_itemName->currentText());
    }



    keyUart=new Model_UART;

    ui->comboBox_COM->addItems(keyUart->PortList());//可用端口列表
    ui->comboBoxCAN1Baud->addItems(QStringList()<< "500K" << "10K" << "20K" << "33.3K" << "40K" << "50K" << "80K" << "83.3K" << "100K" << "125K" << "200K" << "250K" << "400K" << "800K" << "1M");
    ui->comboBoxCAN2Baud->addItems(QStringList()<< "500K" << "10K" << "20K" << "33.3K" << "40K" << "50K" << "80K" << "83.3K" << "100K" << "125K" << "200K" << "250K" << "400K" << "800K" << "1M");

    timeIDSendUart=startTimer(CmdACKDelay);//CmdACKDelay处理一次串口发送或接收

    isStartUartTx=false;
}

ResHardware::~ResHardware()
{

}

/*************************************************************
/函数功能：窗口关闭事件:主窗口调用是全局变量，未能及时释放，因此在关闭窗口处处理
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::closeEvent(QCloseEvent *event)
{
    killTimer(timeIDSendUart);

    delete keyUart;
    delete ui;
    event->accept();
}

void ResHardware::refreshitemName(QString currentText)
{
    QStringList itemList;
    readItemListInfo(itemList);
    ui->comboBox_itemName->clear();
    ui->comboBox_itemName->addItems(itemList);
    ui->comboBox_itemName->setCurrentText(currentText);

}

/*************************************************************
/函数功能：编辑固定按键信息：ACC BAT CCD
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::fixedKeyEdit()
{
    for(int i=0;i<fixedKeyNum;i++)
    {
        keyControl keyInfo;
        getkeyControlNull(&keyInfo);

        //填写具体信息
        switch(i)
        {
        case 0:
            keyInfo.name = "ACC";
            keyInfo.type = HardACC;
            keyInfo.isUse = true;
            break;
        case 1:
            keyInfo.name = "BAT";
            keyInfo.type = HardBAT;
            keyInfo.isUse = true;
            break;
        case 2:
            keyInfo.name = "CCD";
            keyInfo.type = HardCCD;
            keyInfo.isUse = true;
            break;
        }

        keyInfo.des = "name : "+keyInfo.name;
        keyInfo.des +="\ntype : "+getKeyType(keyInfo.type);
        //替换显示及列表信息
        ui->tableWidget->item(i,colDes)->setText(keyInfo.des);
        //key:isEnable
        ui->tableWidget->item(i,colKey)->setCheckState(Qt::Checked);

        //按键信息添加到列表
        keyList.replace(i,keyInfo);
    }
}

/*************************************************************
/函数功能：按键资源初始化
/函数参数：按键信息
/函数返回：无
//keyName->setFlags(keyName->flags() & (~Qt::ItemIsEditable));
*************************************************************/
void ResHardware::tableWidgetInit()
{
    for(int i=0;i<MaxKey;i++)
    {
        keyControl keyInfo;
        getkeyControlNull(&keyInfo);
        int row=ui->tableWidget->rowCount();
        ui->tableWidget->setRowCount(row+1);

        //key：isUse
        QTableWidgetItem *keyName=new QTableWidgetItem("KEY"+QString::number(row+1));
        if(keyInfo.isUse)
            keyName->setCheckState(Qt::Checked);
        else
            keyName->setCheckState(Qt::Unchecked);
        ui->tableWidget->setItem(row,colKey,keyName);

        //key :Description
        QTableWidgetItem *keyDes=new QTableWidgetItem(keyInfo.des);
        ui->tableWidget->setItem(row,colDes,keyDes);

        //key:Edit
        QPushButton *pBtn = new QPushButton(QIcon(":/test/edit.png"),"Edit");
        pBtn->setFlat(true);
        if(row>=fixedKeyNum)
            connect(pBtn, SIGNAL(clicked()), this, SLOT(EditKeyClicked()));
        ui->tableWidget->setCellWidget(row,colEdit,pBtn);

        //key:Width
        ui->tableWidget->setColumnWidth(colKey,80);
        ui->tableWidget->setColumnWidth(colDes,280);

        //key:isEnable
        keyisUsetoEnable(row,keyInfo.isUse);

        //按键信息添加到列表
        keyList.append(keyInfo);
    }

    //any：备注：放在添加项目完成时处理，不然在添加check时会进入
    connect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(on_cellChanged(int,int)));
    connect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),ui->tableWidget,SLOT(resizeRowsToContents()));

    //设置固定按键信息
    fixedKeyEdit();
}

/*************************************************************
/函数功能：按键是否使用，改变信息使用状态
/函数参数：num：设备序列号 isEn：true使能 false失能
/函数返回：无
*************************************************************/
void ResHardware::keyisUsetoEnable(int Num,bool isEn)
{
    //key :Description
    if(isEn)
        ui->tableWidget->item(Num,colDes)->setTextColor(BLACK);
    else
        ui->tableWidget->item(Num,colDes)->setTextColor(GRAY);

    //key:Edit
    QPushButton *tempBtn=(QPushButton*)ui->tableWidget->cellWidget(Num,colEdit);//强制转化为QPushButton
    tempBtn->setEnabled(isEn);
    ui->tableWidget->setCellWidget(Num,colEdit,tempBtn);
}

/*************************************************************
/函数功能：点击编辑信息
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::EditKeyClicked()
{
    QPushButton * senderObj=qobject_cast<QPushButton *>(sender());
    if(senderObj == 0)
        return;

    QModelIndex index =ui->tableWidget->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));

    keyControl theKey=keyList.at(index.row());
    ResHardEdit *hardEdit=new ResHardEdit(&theKey);

    if(hardEdit->exec() == QDialog::Accepted)
    {
        theKey.des = "name : "+theKey.name;
        theKey.des +="\ntype : "+getKeyType(theKey.type);

        if((theKey.type == Can1_1)||(theKey.type == Can2_1))
            theKey.des +="\nCANID : "+theKey.CANID+"\nCANDat : "+theKey.CANDat1;

        else if((theKey.type == Can1_2)||(theKey.type == Can2_2))
            theKey.des +="\nCANID : "+theKey.CANID+"\nCANDatOn : "+theKey.CANDat1+"\nCANDatOff : "+theKey.CANDat2;

        //替换显示及列表信息
        ui->tableWidget->item(index.row(),colDes)->setText(theKey.des);

        keyList.replace(index.row(),theKey);
    }

    delete hardEdit;
}

/*************************************************************
/函数功能：table信息改变：主要处理选中任务
/函数参数：行列
/函数返回：无
/备注：在翻转信息时，若不进行过滤，表格中任何一项更改都将进入此函数，影响数据的处理
*************************************************************/
void ResHardware::on_cellChanged(int row, int column)
{
    if(column == colKey)
    {
        keyControl theKey = keyList.at(row);

        if(ui->tableWidget->item (row,column)->checkState ()==Qt::Checked)
        {
            keyisUsetoEnable(row,true);

            theKey.isUse=true;
        }
        else if(ui->tableWidget->item (row,column)->checkState () == Qt::Unchecked)
        {
            keyisUsetoEnable(row,false);

            theKey.isUse=false;
        }
        keyList.replace(row,theKey);
    }
}

/*************************************************************
/函数功能：读文件中的项目列表
/函数参数：项目
/函数返回：无
*************************************************************/
void ResHardware::readItemListInfo(QStringList &itemList)
{
    Model_XMLFile xmlRead;
    xmlRead.readKeyItemXML(&itemList);
}

/*************************************************************
/函数功能：读项目下的按键信息，并刷新显示
/函数参数：项目
/函数返回：无
*************************************************************/
void ResHardware::readItemKeyInfo(QString item)
{
    Model_XMLFile xmlRead;
    keyList.clear();
    //读xml文件填写
    xmlRead.readKeyInfoXML(item,&keyList);

    if(keyList.isEmpty()==false)
    {
        int row=ui->tableWidget->rowCount();

        for(int i=0;i<row;i++)
        {
            ui->tableWidget->item(i,colDes)->setText(keyList.at(i).des);

            if(keyList.at(i).isUse)
                ui->tableWidget->item (i,colKey)->setCheckState(Qt::Checked);
            else
                ui->tableWidget->item (i,colKey)->setCheckState(Qt::Unchecked);
        }
    }
    else
        cout << "文件中无此按键信息。";
}

void ResHardware::usartTXStatusDeal(bool status,uint8_t transType)
{
    if(status)
    {
        if(keyUart->isOpenCurrentUart()==false)
        {
            QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
            return ;
        }
        //开启传输
        rxCount=0;
        ui->progressBar->setValue(0);
    }
    else
    {
        //结束传输
        ui->progressBar->setValue(100);
    }

    downloadIndex=0;
    isAck=false;
    isDownLoadLast = false;

    isStartUartTx = status;
    gcv_transType = transType;
    ui->groupBox->setEnabled(!status);
    ui->tableWidget->setEnabled(!status);
}

/*************************************************************
/函数功能：上传数据信息，从小板获取项目号，再从文件中读取该项目的按键内容
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::on_pushButton_inDat_clicked()
{
    usartTXStatusDeal(true,usart_UpKeyInfo);
}

/*************************************************************
/函数功能：下载按键信息
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::on_pushButton_outDat_clicked()
{
    if(QMessageBox::information(NULL, tr("下载"), tr("项目名：")+ui->comboBox_itemName->currentText(), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)//Upgrading
    {
        usartTXStatusDeal(true,usart_DownKeyInfo);
    }
}

/*************************************************************
/函数功能：切换项目编号，刷新项目列表：因此在需要的地方只需更改项目编号而已，无需再操心刷新问题
/函数参数：项目
/函数返回：无
*************************************************************/
void ResHardware::itemNameSlot(const QString &arg1)
{
    readItemKeyInfo(arg1);
    itemNameChange(arg1);
}

/*************************************************************
/函数功能：重置列表
/函数参数：无
/函数返回：无
/备注：在删除项目时断开连接，避免重新添加时进行信号处理
*************************************************************/
void ResHardware::on_pushButton_reset_clicked()
{
    keyList.clear();

    int row = ui->tableWidget->rowCount();
    for(uint16_t i=row;i>0;i--)
    {
        ui->tableWidget->removeRow(i-1);
    }
    ui->tableWidget->disconnect();//删除时同时去掉连接，避免重新初始化时进入信号处理函数

    tableWidgetInit();
}



/*************************************************************
/函数功能：定时处理函数
/函数参数：event：定时器事件
/函数返回：无
*************************************************************/
void ResHardware::timerEvent(QTimerEvent *event)
{
    if((event->timerId()==timeIDSendUart)&&(isStartUartTx))
    {
        switch(gcv_transType)
        {
        case usart_DownKeyInfo:
        {
            if((!isAck)&&(downloadIndex))
            {
                //响应延时3倍时间等待，若仍无响应终止传输:在200ms内，串口函数中将再次发送
                if((++txCount) > (CmdReSendTimer+2))
                {
                    QMessageBox::warning(NULL, tr("提示"), tr("串口传输无响应，请检查串口！"));
                    txCount=0;
                    downloadIndex=0;
                    usartTXStatusDeal(false,usart_NONE);
                    ui->label_Show->setText(tr("下载失败！"));
                }
            }
            else
                downUartDeal();
            break;
        }
        case usart_UpKeyInfo:
        {
            upUartDeal();
            break;
        }
        case usart_SignalInfo:
        {
            if(!isAck)
            {
                //响应延时3倍时间等待，若仍无响应终止传输:在200ms内，串口函数中将再次发送
                if((++txCount) > (CmdReSendTimer+2))
                {
                    QMessageBox::warning(NULL, tr("提示"), tr("串口传输无响应，请检查串口！"));
                    txCount=0;
                    usartTXStatusDeal(false,usart_NONE);
                    ui->label_Show->setText(tr("通道处理失败！"));
                }
            }
            else
            {
                ui->label_Show->setText(tr("通道处理完成！"));
                usartTXStatusDeal(false,usart_NONE);
            }
            break;
        }
        default:
            isStartUartTx=false;
            break;
        }
    }
}

/*************************************************************
/函数功能：串口接收处理函数
/函数参数：
/函数返回：无
*************************************************************/
void ResHardware::UartRxDealSlot(char cmd,uint8_t dLen,char *dat)
{
    if(cmd == CMDItemRead)
    {
        QString item(dat);
        cout << item << dLen;

        //ui->label_inItem->setText(item);

        rxCount=0;

        //this->close();any:处理项目接收解析
    }
}

/*************************************************************
/函数功能：接收串口响应处理槽函数
/函数参数：ack：响应结果
/函数返回：无
*************************************************************/
void ResHardware::UartRxAckResault(bool ack)
{
    if(!ack)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("串口响应失败，请检查串口！"));
        txCount=0;
        downloadIndex=0;
        usartTXStatusDeal(false,usart_NONE);
        ui->label_Show->setText(tr("传输失败！"));
    }
    else
        isAck=true;
}


/*************************************************************
/函数功能：串口下载处理
/函数参数：无
/函数返回：无
//传递项目号 + 按键信息 + 保存按键操作
*************************************************************/
void ResHardware::downUartDeal()
{
    //cout << downloadIndex;
    char buf[BUFSIZ]={0};//不可用指针，当数据较多时无法正确赋值，因未分配内存，照成数据混乱
    int len = 0;

    QByteArray arrayBuf ;

    if(!downloadIndex)
    {
        //下载项目号
        arrayBuf = ui->comboBox_itemName->currentText().toLatin1();

        len = arrayBuf.length();
        strcpy(buf,arrayBuf);

        ui->label_Show->setText(tr("下载:项目： ")+arrayBuf);
        ui->progressBar->setValue(ui->progressBar->value()+proValueAdd);
        keyUart->UartTxCmdDeal(CMDItemWrite,buf,len,CMD_NEEDACK);
    }
    else if(keyList.length()>(downloadIndex-1))
    {
        //下载按键信息
        Model_String strDeal;
        keyControl keyInfo = keyList.at(downloadIndex-1);

        arrayBuf = keyInfo.name.toLatin1();

        buf[0]=downloadIndex-1;
        strcpy(&buf[1],arrayBuf);
        len = arrayBuf.length()+1;

        buf[len++] = '*';
        buf[len++]=keyInfo.isUse;
        buf[len++]=keyInfo.type;

        //协议数据传输
        if(keyInfo.type>HardRes)//((keyInfo.type == Can1_1) || (keyInfo.type == Can1_2))
        {
            QString idStr;
            arrayBuf.clear();

            //数据转换时奇数个将在后面补0，因此提前处理
            if(keyInfo.CANID.length()%2)
                idStr='0'+keyInfo.CANID;
            else
                idStr=keyInfo.CANID;
            strDeal.StringToHex(idStr,arrayBuf);
            for(int i=0;i<4;i++)
            {
                if(i<(4-arrayBuf.length()))
                    buf[len++] = 0x00;
                else
                    buf[len++] = arrayBuf[i-(4-arrayBuf.length())];
            }

            arrayBuf.clear();

            strDeal.StringToHex(keyInfo.CANDat1,arrayBuf);
            for(int i=0;i<8;i++)
            {
                if(i<arrayBuf.length())
                    buf[len++] = arrayBuf[i];
                else
                    buf[len++] = 0x00;
            }

            if((keyInfo.type == Can1_2)||(keyInfo.type == Can2_2))
            {
                arrayBuf.clear();

                strDeal.StringToHex(keyInfo.CANDat2,arrayBuf);
                for(int i=0;i<8;i++)
                {
                    if(i<arrayBuf.length())
                        buf[len++] = arrayBuf[i];
                    else
                        buf[len++] = 0x00;
                }
            }
        }
        ui->label_Show->setText(tr("下载:按键：")+QString::number(downloadIndex)+"  "+keyInfo.name);
        ui->progressBar->setValue(ui->progressBar->value()+proValueAdd);
        keyUart->UartTxCmdDeal(CMDDownloadKey,buf,len,CMD_NEEDACK);
    }
    else
    {
        if(!isDownLoadLast)
        {
            //下发保存指令
            cout <<"save keyinfo";
            keyUart->UartTxCmdDeal(CMDSaveKeyInfo,buf,len,CMD_NEEDACK);
            isDownLoadLast=true;
        }
        else
        {
            //下载完成
            //cout << downloadIndex;
            ui->label_Show->setText(tr("下载结束，请等待小板保存完成..."));
            usartTXStatusDeal(false,usart_NONE);
            return;
        }

    }
    isAck=false;
    txCount=0;
    downloadIndex++;
}

/*************************************************************
/函数功能：串口上传处理
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::upUartDeal()
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

            ui->pushButton_inDat->setEnabled(true);
        }
    }
    rxCount++;
}

/*************************************************************
/函数功能：保存按键信息
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::on_pushButtonSave_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("下载"),tr("项目名:"), QLineEdit::Normal,ui->comboBox_itemName->currentText(), &ok);

    if (ok && !text.isEmpty())
    {
        Model_XMLFile xmlSave;
        QStringList keylist;
        QString itemName = text;

        bool isAppend=true;

        xmlSave.createKeyInfoXML();

        if(xmlSave.hasItemKeyInfomation(itemName))
        {
            if(QMessageBox::information(NULL, tr("提示"), tr("文件中该项目已存在，是否替换按键定义？？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)
            {
                return;
            }
            isAppend=false;
        }
        ui->label_Show->setText(tr("保存:保存按键信息到《KeyInfo.xml》 文件中. "));

        for(int i=0;i<keyList.length();i++)
        {
            keyControl keyInfo = keyList.at(i);
            keylist.clear();

            keylist << "KEY"+QString::number(i+1) << keyInfo.name << QString::number(keyInfo.isUse) << keyInfo.des << QString::number(keyInfo.type) << keyInfo.CANID << keyInfo.CANDat1 << keyInfo.CANDat2;

            xmlSave.appendKeyInfoXML(itemName,isAppend,keylist);
        }
        refreshitemName(itemName);
    }
}

/*************************************************************
/函数功能：打开或关闭串口
/函数参数：checked  true :开  false:关
/函数返回：无
*************************************************************/
void ResHardware::on_checkBoxENUart_clicked(bool checked)
{
    if(!checked)
    {
        keyUart->Close();//关串口
        ui->comboBox_COM->setEnabled(true);
        ui->comboBox_BAUD->setEnabled(true);
        ui->groupBoxUARTSEND->setEnabled(false);
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
            ui->comboBox_COM->setEnabled(false);
            ui->comboBox_BAUD->setEnabled(false);
            ui->groupBoxUARTSEND->setEnabled(true);
        }
    }
}
/*************************************************************
/函数功能：CAN波特率转换
/函数参数：字符串
/函数返回：数值
*************************************************************/
uint16_t ResHardware::covCANBaudDeal(QString baud)
{
    uint16_t Dat_temp=0;
    if(baud=="10K")       Dat_temp=10;
    else if(baud=="20K")  Dat_temp=20;
    else if(baud=="33.3K")Dat_temp=33;
    else if(baud=="40K")  Dat_temp=40;
    else if(baud=="50K")  Dat_temp=50;
    else if(baud=="80K")  Dat_temp=80;
    else if(baud=="83.3K")Dat_temp=83;
    else if(baud=="100K") Dat_temp=100;
    else if(baud=="125K") Dat_temp=125;
    else if(baud=="200K") Dat_temp=200;
    else if(baud=="250K") Dat_temp=250;
    else if(baud=="400K") Dat_temp=400;
    else if(baud=="500K") Dat_temp=500;
    else if(baud=="800K") Dat_temp=800;
    else if(baud=="1M")   Dat_temp=1000;
    return Dat_temp;
}
/*************************************************************
/函数功能：打开或关闭CAN1
/函数参数：checked  true :开  false:关
/函数返回：无
*************************************************************/
void ResHardware::on_checkBoxENCAN1_clicked(bool checked)
{
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        ui->checkBoxENCAN1->setChecked(!checked);
        return ;
    }

    if(checked)
    {
        ui->comboBoxCAN1Baud->setEnabled(false);
        ui->comboBoxCAN1Type->setEnabled(false);
    }
    else
    {
        ui->comboBoxCAN1Baud->setEnabled(true);
        ui->comboBoxCAN1Type->setEnabled(true);
    }

    usartTXStatusDeal(true,usart_SignalInfo);

    uint16_t CANbaud = covCANBaudDeal(ui->comboBoxCAN1Baud->currentText());
    char buf[BUFSIZ]={0};

    buf[0] = checked;

    if(ui->comboBoxCAN1Type->currentIndex()==0)//类型
        buf[1]=CAN1CHANNEL;
    else
        buf[1]=CANSingle;

    buf[2]=CANbaud>>8;
    buf[3]=CANbaud&0x00ff;

    keyUart->UartTxCmdDeal(CMDCANChannel,buf,4,CMD_NEEDACK);
    isAck=false;
    txCount=0;
    ui->label_Show->setText(tr("下载:CAN1通道配置"));
}
/*************************************************************
/函数功能：打开或关闭CAN2
/函数参数：checked  true :开  false:关
/函数返回：无
*************************************************************/
void ResHardware::on_checkBoxENCAN2_clicked(bool checked)
{
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        ui->checkBoxENCAN2->setChecked(!checked);
        return ;
    }
    if(checked)
    {
        ui->comboBoxCAN2Baud->setEnabled(false);
        ui->comboBoxCAN2Type->setEnabled(false);
    }
    else
    {
        ui->comboBoxCAN2Baud->setEnabled(true);
        ui->comboBoxCAN2Type->setEnabled(true);
    }
    usartTXStatusDeal(true,usart_SignalInfo);
    uint16_t CANbaud = covCANBaudDeal(ui->comboBoxCAN2Baud->currentText());
    char buf[BUFSIZ]={0};

    buf[0] = checked;
    buf[1] = CAN2CHANNEL;
    buf[2]=CANbaud>>8;
    buf[3]=CANbaud&0x00ff;
    keyUart->UartTxCmdDeal(CMDCANChannel,buf,4,CMD_NEEDACK);
    isAck=false;
    txCount=0;
    ui->label_Show->setText(tr("下载:CAN2通道配置"));
}

/*************************************************************
/函数功能：设置BAT电压值
/函数参数：值
/函数返回：无
*************************************************************/
void ResHardware::on_horizontalSliderBAT_valueChanged(int value)
{
    ui->label_ShowBATVal->setText(toStr(value));
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        return ;
    }
    usartTXStatusDeal(true,usart_SignalInfo);
    char buf[BUFSIZ]={0};

    buf[0] = value;
    keyUart->UartTxCmdDeal(CMDBATPower,buf,1,CMD_NEEDACK);
    isAck=false;
    txCount=0;
    ui->label_Show->setText(tr("下载:Bat电压值")+toStr(value));
}
/*************************************************************
/函数功能：设置CCD电压值
/函数参数：值
/函数返回：无
*************************************************************/
void ResHardware::on_horizontalSliderCCD_valueChanged(int value)
{
    ui->label_ShowCCDVal->setText(toStr(value));
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        return ;
    }
    usartTXStatusDeal(true,usart_SignalInfo);
    char buf[BUFSIZ]={0};

    buf[0] = value;
    keyUart->UartTxCmdDeal(CMDCCDPower,buf,1,CMD_NEEDACK);
    isAck=false;
    txCount=0;
    ui->label_Show->setText(tr("下载:摄像头电压值")+toStr(value));
}
/*************************************************************
/函数功能：设置15V最大电压值
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::on_radioBtn15V_clicked()
{
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        ui->radioBtn24V->setChecked(true);
        return ;
    }
    usartTXStatusDeal(true,usart_SignalInfo);
    char buf[BUFSIZ]={0};

    buf[0] = false;
    keyUart->UartTxCmdDeal(CMDBATMaxVal,buf,1,CMD_NEEDACK);
    isAck=false;
    txCount=0;
    ui->label_Show->setText(tr("下载:Bat最大工作电压"));

    ui->horizontalSliderBAT->setMaximum(15);
    ui->horizontalSliderCCD->setMaximum(15);
}
/*************************************************************
/函数功能：设置24V最大电压值
/函数参数：wu
/函数返回：无
*************************************************************/
void ResHardware::on_radioBtn24V_clicked()
{
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        ui->radioBtn15V->setChecked(true);
        return ;
    }
    usartTXStatusDeal(true,usart_SignalInfo);
    char buf[BUFSIZ]={0};

    buf[0] = true;
    keyUart->UartTxCmdDeal(CMDBATMaxVal,buf,1,CMD_NEEDACK);
    isAck=false;
    txCount=0;
    ui->label_Show->setText(tr("下载:Bat最大工作电压"));

    ui->horizontalSliderBAT->setMaximum(24);
    ui->horizontalSliderCCD->setMaximum(24);
}

