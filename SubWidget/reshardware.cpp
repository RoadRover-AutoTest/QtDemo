#include "reshardware.h"
#include "ui_reshardware.h"

QStringList upKeysInfo;

ResHardware::ResHardware(Model_UART *uart, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResHardware)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() | Qt::WindowMinMaxButtonsHint);

    //小屏显示
    strDeal = new Model_String();
    strDeal->MediaTransModel(0);//默认数据不转换
    connect(strDeal,SIGNAL(UpdateScreenSignal(unsigned char)),this,SLOT(UpdateScreenSlot(unsigned char)));

    QPalette mainTextEditPalette;//设置小屏界面背景和字体颜色
    mainTextEditPalette.setColor(QPalette::Text,QColor(90,232,248));
    ui->mainTextEdit->setPalette(mainTextEditPalette);
    mainTextEditPalette.setColor(QPalette::Base,QColor(20,20,20));
    ui->mainTextEdit->setPalette(mainTextEditPalette);

    //项目及按键
    refreshitemName(WorkItem);          //刷新项目名列表：从文件中获取项目信息
    initkeyList();                      //初始化点击按键列表
    tableWidgetInit();                  //初始化编辑列表框
    getkeyControlNull(&copyTempKey);    //初始化拷贝临时变量信息

    //初始化当前项目的按键
    if(ui->comboBox_itemName->currentText().isEmpty() == false)
    {
        readItemKeyInfo(ui->comboBox_itemName->currentText());
    }

    //定义串口
    keyUart = uart;//=new Model_UART;  //any:由主函数定义
    connect(keyUart,SIGNAL(RxFrameDat(char,uint8_t,char*)),this,SLOT(UartRxDealSlot(char,uint8_t,char*)));
    connect(keyUart,SIGNAL(UartRxAckResault(bool)),this,SLOT(UartRxAckResault(bool)));
    connect(keyUart,SIGNAL(UartError()),this,SLOT(UartErrorDeal()));
    connect(keyUart,SIGNAL(UartByteArrayBackStage(QByteArray,uartDir,bool)),this,SLOT(UartByteArraySlot(QByteArray,uartDir,bool)));

    //添加链接
    connect(ui->comboBox_itemName,SIGNAL(activated(QString)),this,SLOT(itemNameSlot(QString)));
    connect(ui->comboBox_itemName,SIGNAL(currentTextChanged(QString)),this,SLOT(itemNameSlot(QString)));

    connect(this,SIGNAL(keysUploadOver()),this,SLOT(uploadKeysDeal()));

    uartReadCANParamDeal();

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
    keyUart->Close();//关串口
    delete strDeal;
    delete keyUart;
    delete ui;
    event->accept();
    windowClose();
}

/*************************************************************
/函数功能：刷新项目名
/函数参数：项目名
/函数返回：无
*************************************************************/
void ResHardware::refreshitemName(QString currentText)
{
    QStringList itemList;
    Model_XMLFile xmlRead;
    xmlRead.readKeyItemXML(&itemList);
    ui->comboBox_itemName->clear();
    ui->comboBox_itemName->addItems(itemList);
    ui->comboBox_itemName->setCurrentText(currentText);

}

/*************************************************************
/函数功能：初始化按键列表
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::initkeyList()
{
    Model_XMLFile xmlRead;
    QList <keyControl> keysClickList;
    QStringList comboList;

    xmlRead.readKeyInfoXML(WorkItem,&keysClickList);

    if(keysClickList.isEmpty()==false)
    {
        for(int i=0;i<keysClickList.length();i++)
        {
            if(keysClickList.at(i).isUse)
            {
                comboList.append("KEY"+QString::number(i+1)+":"+keysClickList.at(i).name);

                //标记特殊按键处理字符串：
                if(keysClickList.at(i).type == HardACC)
                {
                    AccKey = comboList.last();
                }
                else if(keysClickList.at(i).type == HardBAT)
                {
                    BatKey = comboList.last();
                }
            }
        }
    }
    ui->comboBoxKeyList->clear();
    ui->comboBoxKeyList->addItems(comboList);
}

/*---------------------------------------this is KeyEdit option-----------------------------------------*/
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
    keyList.clear();
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

/*************************************************************
/函数功能：切换项目编号，刷新项目列表：因此在需要的地方只需更改项目编号而已，无需再操心刷新问题
/函数参数：项目
/函数返回：无
*************************************************************/
void ResHardware::itemNameSlot(const QString &arg1)
{
    readItemKeyInfo(arg1);
    itemNameChange(arg1);
    initkeyList();
}

/*************************************************************
/函数功能：重置列表
/函数参数：无
/函数返回：无
/备注：在删除项目时断开连接，避免重新添加时进行信号处理
*************************************************************/
void ResHardware::customContextMenuReset_clicked()
{
    keyList.clear();

    int row = ui->tableWidget->rowCount();
    for(uint16_t i=row;i>0;i--)
    {
        ui->tableWidget->removeRow(i-1);
    }
    //ui->tableWidget->disconnect();//any:不使用全局取消链接操作，以免将其余链接无法使用（菜单链接）
    disconnect(ui->tableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(on_cellChanged(int,int)));//删除时同时去掉连接，避免重新初始化时进入信号处理函数
    disconnect(ui->tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),ui->tableWidget,SLOT(resizeRowsToContents()));
    tableWidgetInit();
}

/*************************************************************
/函数功能：保存按键信息
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::customContextMenuSave_clicked()
{
    saveKeysInfoToXML(ui->comboBox_itemName->currentText());
}

/*************************************************************
/函数功能：保存按键信息
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::saveKeysInfoToXML(QString itemName)
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("保存"),tr("项目名:"), QLineEdit::Normal,itemName, &ok);

    if (ok && !text.isEmpty())
    {
        Model_XMLFile xmlSave;
        QStringList keylist;
        itemName = text;

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
        initkeyList();
    }

}

/*************************************************************
/函数功能：复制按键信息
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::customContextMenuCopy_clicked()
{
    //copyTempKey
    QTableWidgetItem * item = ui->tableWidget->currentItem();
    if( item == NULL )
        return;

    int curIndex = ui->tableWidget->row(item);
    copyTempKey = keyList.at(curIndex);
}

/*************************************************************
/函数功能：粘贴按键信息
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::customContextMenuPaste_clicked()
{
    QTableWidgetItem * item = ui->tableWidget->currentItem();
    if( item == NULL )
        return;

    int curIndex = ui->tableWidget->row(item);
    keyList.replace(curIndex,copyTempKey);
    //替换显示及列表信息
    ui->tableWidget->item(curIndex,colDes)->setText(copyTempKey.des);
    //key:isEnable
    if(copyTempKey.isUse)
        ui->tableWidget->item (curIndex,colKey)->setCheckState(Qt::Checked);
    else
        ui->tableWidget->item (curIndex,colKey)->setCheckState(Qt::Unchecked);
}

/*************************************************************
/函数功能：删除按键信息
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::customContextMenuDelete_clicked()
{
    //copyTempKey
    QTableWidgetItem * item = ui->tableWidget->currentItem();
    if( item == NULL )
        return;

    int curIndex = ui->tableWidget->row(item);
    keyControl keyInfo;
    getkeyControlNull(&keyInfo);
    //替换显示及列表信息
    ui->tableWidget->item(curIndex,colDes)->setText(keyInfo.des);
    //key:isEnable
    ui->tableWidget->item(curIndex,colKey)->setCheckState(Qt::Unchecked);
    keyList.replace(curIndex,keyInfo);

}

/*************************************************************
/函数功能：tableWidget右键创建菜单栏
/函数参数：位置
/函数返回：wu
*************************************************************/
void ResHardware::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{
    QTableWidgetItem * item = ui->tableWidget->currentItem();
    if( item == NULL )
        return;

    int curIndex = ui->tableWidget->row(item);
    Q_UNUSED(pos);
    QMenu *popMenu = new QMenu( this );
    QAction *saveAct = new QAction(tr("保存到KeyInfo.xml"), this);
    QAction *downloadAct = new QAction(QIcon(":/Title/download.png"),tr("下载"), this);
    QAction *uploadAct = new QAction(QIcon(":/Title/upload.png"),tr("上传"), this);
    QAction *copyAct = new QAction(tr("复制")+"    KEY"+toStr(curIndex+1), this);
    QAction *pasteAct = new QAction(tr("粘贴到")+"  KEY"+toStr(curIndex+1), this);
    QAction *resetAct = new QAction(tr("重置列表"), this);
    QAction *deleteAct = new QAction(tr("删除")+"    KEY"+toStr(curIndex+1), this);

    //添加到主菜单：
    if(userLogin.Permissions == Administrator)
    {
        popMenu->addAction(saveAct);
        popMenu->addAction(resetAct);
        popMenu->addAction(downloadAct);
        popMenu->addAction(uploadAct);
        popMenu->addSeparator();
        popMenu->addAction(copyAct);
        popMenu->addAction(pasteAct);
        popMenu->addAction(deleteAct);


        if(keyList.at(curIndex).name.isEmpty())
        {
            copyAct->setEnabled(false);//当前项无数据，无复制和删除操作
            deleteAct->setEnabled(false);
        }
        if(copyTempKey.name.isEmpty())
            pasteAct->setEnabled(false);//复制的临时数据为空，无粘贴操作
        if(keyUart->isOpenCurrentUart()==false)
        {
            downloadAct->setEnabled(false);//串口未打开，无下载，上传操作
            uploadAct->setEnabled(false);
        }
    }

    connect(saveAct,SIGNAL(triggered()),this,SLOT(customContextMenuSave_clicked()));
    connect(resetAct,SIGNAL(triggered()),this,SLOT(customContextMenuReset_clicked()));
    connect(uploadAct,SIGNAL(triggered()),this,SLOT(customContextMenuUpload_clicked()));
    connect(downloadAct,SIGNAL(triggered()),this,SLOT(customContextMenuDownload_clicked()));
    connect(copyAct,SIGNAL(triggered()),this,SLOT(customContextMenuCopy_clicked()));
    connect(pasteAct,SIGNAL(triggered()),this,SLOT(customContextMenuPaste_clicked()));
    connect(deleteAct,SIGNAL(triggered()),this,SLOT(customContextMenuDelete_clicked()));
    popMenu->exec( QCursor::pos() );

    delete popMenu;

}


/*---------------------------------------this is UartDeal option-----------------------------------------*/
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

        if(QMessageBox::information(NULL, tr("提示"), tr("获取到项目：")+item+tr("\n是否应用，且继续获取按键信息？？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)
        {
            QStringList itemList;
            Model_XMLFile xmlRead;
            xmlRead.readKeyItemXML(&itemList);

            if(itemList.contains(item))
                ui->comboBox_itemName->setCurrentText(item);

            char read=0;
            keyUart->appendTxList(CMDUploadKey,&read,1,CMD_NEEDNACK);
            ui->label_Show->setText(tr("读取：按键信息"));
            g_ItemReadString = item;
        }

        ui->label_Show->setText(tr("项目：")+item);
    }
    else if(cmd == CMDUploadKey)
    {
        uint8_t keyIndex = dat[0];
        QString keyInfo = QString::fromUtf8(&dat[1],dLen-1);
        if(keyIndex == 0)
            upKeysInfo.clear();

        upKeysInfo.append(keyInfo);
        //cout <<keyIndex<<keyInfo;

        //最后一帧按键时处理显示
        if(keyIndex == (MaxKey-1))
        {
            keysUploadOver();
        }
    }
    else if(cmd == CMDUploadBatVal)
    {
        unsigned int tempDat=0;
        for(int i=0;i<dLen;i++)
        {
            tempDat=((uint8_t)dat[i]<<(i*8))|tempDat;//低位在前，高位在后
        }
        ui->lineEditShowVB->setText(toStr(tempDat/100.0)+"V");
    }
    else if(cmd == CMD_MEDIA_INFO)//媒体信息
    {
        DispBufClear();

        strDeal->MediaInfoAnalyze(dLen,dat,g_DisplayBuf);//,strDeal->Uni2UTF8_LittleEndian);
    }
    else if(cmd == CMD_MEDIA_INFO2)//媒体信息2
    {
        DispBufClear();
        strDeal->MediaInfo2Analyze(dLen,dat,g_DisplayBuf);//,strDeal->Uni2UTF8_LittleEndian);
    }
    else if(cmd == CMD_LINK_STATUS)//连接状态信息
    {
        if(dat[0] == 0x01)
        {
            ui->linkStatusRadioButton->setText("设备已连接！");
            ui->linkStatusRadioButton->setChecked(true);
        }
        else if(dat[0] == 0x00)
        {
            ui->linkStatusRadioButton->setText("设备未连接！");
            ui->linkStatusRadioButton->setChecked(false);
        }
    }
    else if(cmd == CMDUploadCAN)//CAN 参数
    {
        //帧结构：Byte0：开/关   Byte1：CAN类型    Byte2~3：CAN波特率
        if((dat[0]==1) ||(dat[0]==0))//避免其他数据结果乱入（从测试板内存中读取数据出错的情况，或初始情况）
        {
            if(dat[1] == CAN2CHANNEL)
            {
                if(dat[0])
                {
                    ui->checkBoxENCAN2->setChecked(true);
                    ui->comboBoxCAN2Baud->setEnabled(false);
                    ui->comboBoxCAN2Type->setEnabled(false);
                }
                else
                {
                    ui->checkBoxENCAN2->setChecked(false);
                    ui->comboBoxCAN2Baud->setEnabled(true);
                    ui->comboBoxCAN2Type->setEnabled(true);
                }


                uint16_t baud = dat[2]<<8|dat[3];
                ui->comboBoxCAN2Baud->setCurrentText( recovCANBaudDeal(baud));
            }
            else
            {
                if(dat[1] == CAN1CHANNEL)
                {
                    ui->comboBoxCAN1Type->setCurrentIndex(0);
                }
                else
                    ui->comboBoxCAN1Type->setCurrentIndex(1);

                if(dat[0])
                {
                    ui->checkBoxENCAN1->setChecked(true);
                    ui->comboBoxCAN1Baud->setEnabled(false);
                    ui->comboBoxCAN1Type->setEnabled(false);
                }
                else
                {
                    ui->checkBoxENCAN1->setChecked(false);
                    ui->comboBoxCAN1Baud->setEnabled(true);
                    ui->comboBoxCAN1Type->setEnabled(true);
                }


                uint16_t baud = dat[2]<<8|dat[3];
                ui->comboBoxCAN1Baud->setCurrentText( recovCANBaudDeal(baud));
            }

        }

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
        cout<<("串口响应失败，请检查！");
        ui->label_Show->setText(tr("传输失败！"));

    }
}

/*************************************************************
/函数功能：串口错误处理：串口断开，响应失败
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::UartErrorDeal()
{
    QMessageBox::warning(NULL, tr("提示"), tr("串口错误，请检查串口！"));
    ui->label_Show->setText(tr("通道处理失败！"));
}

/*************************************************************
/函数功能：串口字符串后台显示数据传输
/函数参数：revDats:接收字符串数据 dir：传输方向  isHex：是否为hex数据
/函数返回：无
*************************************************************/
void ResHardware::UartByteArraySlot(QByteArray revDats,uartDir dir,bool isHex)
{
    QString strShow;

    if(isHex==Uart_Hex)
        strShow=strDeal->hexToString((unsigned char *)revDats.data(),revDats.length());//hex显示
    else if(isHex==Uart_NHex)
        strShow=revDats;

    if(dir==Uart_Rx)
        ui->textBrowserFrameShow->append("Rx: "+strShow);
    else if(dir==Uart_Tx)
        ui->textBrowserFrameShow->append("Tx: "+strShow);
    else
        ui->textBrowserFrameShow->append("Warn: "+strShow);
}

/*---------------------------------------this is 小屏显示处理 option-----------------------------------------*/
/*************************************************************
/函数功能：清空显示缓存
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::DispBufClear()
{
    for(unsigned char i = 0;i < 8;i++)
        g_DisplayBuf[i] = "                              ";
}

/*************************************************************
/函数功能：更新屏显示槽函数
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::UpdateScreenSlot(unsigned char line)
{
    ui->mainTextEdit->clear();
    ui->mainTextEdit->append("<font color=yellow>" + g_DisplayBuf[0] + "</font>");//添加HTML语句，使第一行文字为黄色
    //ui->mainTextEdit->append(g_DisplayBuf[0]);
    for(int i = 1; i <= line; i++)
    {
        if(g_DisplayBuf[i].length()>64)//字符串长度不超过64个
            g_DisplayBuf[i] = g_DisplayBuf[i].left(64);
        ui->mainTextEdit->append(g_DisplayBuf[i]);
    }
}

/*************************************************************
/函数功能：更改数据转换模式
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::on_comboBox_MediaTrans_currentIndexChanged(int index)
{
    strDeal->MediaTransModel(index);
}



/*---------------------------------------this is 组件处理函数 option-----------------------------------------*/
/*************************************************************
/函数功能：上传数据信息，从小板获取项目号，再从文件中读取该项目的按键内容
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::customContextMenuUpload_clicked()
{
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        return ;
    }

    char read=0;
    keyUart->appendTxList(CMDItemRead,&read,1,CMD_NEEDNACK);
    ui->label_Show->setText(tr("读取：项目型号"));
}

/*************************************************************
/函数功能：上传数据信息解析
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::uploadKeysDeal()
{
    if(upKeysInfo.length()==MaxKey)
    {
        //解析
        for(int count=0;count < MaxKey;count++)
        {
            int index;
            char *dat;
            QByteArray bt = upKeysInfo.at(count).toLatin1();
            dat = bt.data();

            keyControl keyInfo;
            getkeyControlNull(&keyInfo);

            for(index=0;index<bt.length();index++)
            {
                if(dat[index]!='*')
                    keyInfo.name.append(dat[index]);
                else
                {
                    index++;
                    break;
                }

            }
            keyInfo.isUse = dat[index++];
            keyInfo.type = (kType)dat[index++];
            if(keyInfo.isUse)
            {
                keyInfo.des = "name : "+keyInfo.name;
                keyInfo.des +="\ntype : "+getKeyType(keyInfo.type);
            }

            //any：处理延时任务，照成接收失误，换地方处理
            if((keyInfo.type == Can1_1)||(keyInfo.type == Can1_2)||(keyInfo.type == Can2_1)||(keyInfo.type == Can2_2))
            {
                keyInfo.CANID=strDeal->hexToString((unsigned char *)&dat[index],4);
                index+=4;

                keyInfo.CANDat1=strDeal->hexToString((unsigned char *)&dat[index],8);
                index+=8;
                keyInfo.des +="\nCANID : "+keyInfo.CANID+"\nCANDat : "+keyInfo.CANDat1;


                if((keyInfo.type==Can1_2)||(keyInfo.type==Can2_2))
                {
                    keyInfo.CANDat2=strDeal->hexToString((unsigned char *)&dat[index],8);
                    keyInfo.des +="\nCANDatOff : "+keyInfo.CANDat2;
                }
            }
            keyList.replace(count,keyInfo);
            cout <<keyInfo.name<<keyInfo.isUse <<keyInfo.type<<keyInfo.CANID<<keyInfo.CANDat1<<keyInfo.CANDat2;
        }

        //显示
        int row=ui->tableWidget->rowCount();

        for(int i=0;i<row;i++)
        {
            ui->tableWidget->item(i,colDes)->setText(keyList.at(i).des);

            if(keyList.at(i).isUse)
                ui->tableWidget->item (i,colKey)->setCheckState(Qt::Checked);
            else
                ui->tableWidget->item (i,colKey)->setCheckState(Qt::Unchecked);
        }

        initkeyList();                      //初始化点击按键列表

        //保存
        saveKeysInfoToXML(g_ItemReadString);
    }
}

/*************************************************************
/函数功能：下载按键信息
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::customContextMenuDownload_clicked()
{
    //串口是否连接
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        return ;
    }

    //下载项目及对应的按键:
    if(QMessageBox::information(NULL, tr("下载"), tr("项目名：")+ui->comboBox_itemName->currentText(), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::Yes)//Upgrading
    {
        uint8_t downloadIndex = 0;
        bool isDownLoadLast = false;


        while(1)
        {
            //cout << downloadIndex;
            char buf[BUFSIZ]={0};//不可用指针，当数据较多时无法正确赋值，因未分配内存，照成数据混乱
            int len = 0;

            QByteArray arrayBuf ;

            if(!downloadIndex)
            {
                arrayBuf = ui->comboBox_itemName->currentText().toLatin1();//下载项目号

                len = arrayBuf.length();
                strcpy(buf,arrayBuf);

                ui->label_Show->setText(tr("下载:项目： ")+arrayBuf);
                keyUart->appendTxList(CMDItemWrite,buf,len,CMD_NEEDACK);
            }
            else if(keyList.length()>(downloadIndex-1))
            {
                //Model_String strDeal;
                keyControl keyInfo = keyList.at(downloadIndex-1);//下载按键信息

                arrayBuf = keyInfo.name.toLatin1();

                buf[0]=downloadIndex-1;
                strcpy(&buf[1],arrayBuf);
                len = arrayBuf.length()+1;

                buf[len++] = '*';
                buf[len++]=keyInfo.isUse;
                buf[len++]=keyInfo.type;

                //协议数据传输
                if((keyInfo.type == Can1_1) || (keyInfo.type == Can1_2)||(keyInfo.type == Can2_1) || (keyInfo.type == Can2_2))
                {
                    QString idStr;
                    arrayBuf.clear();

                    //数据转换时奇数个将在后面补0，因此提前处理
                    if(keyInfo.CANID.length()%2)
                        idStr='0'+keyInfo.CANID;
                    else
                        idStr=keyInfo.CANID;
                    strDeal->StringToHex(idStr,arrayBuf);
                    for(int i=0;i<4;i++)
                    {
                        if(i<(4-arrayBuf.length()))
                            buf[len++] = 0x00;
                        else
                            buf[len++] = arrayBuf[i-(4-arrayBuf.length())];
                    }

                    arrayBuf.clear();

                    strDeal->StringToHex(keyInfo.CANDat1,arrayBuf);
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

                        strDeal->StringToHex(keyInfo.CANDat2,arrayBuf);
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
                keyUart->appendTxList(CMDDownloadKey,buf,len,CMD_NEEDACK);
            }
            else
            {
                if(!isDownLoadLast)
                {
                    //下发保存指令
                    cout <<"save keyinfo";
                    keyUart->appendTxList(CMDSaveKeyInfo,buf,len,CMD_NEEDACK);
                    isDownLoadLast=true;
                }
                else
                {
                    //下载完成
                    //cout << downloadIndex;
                    ui->label_Show->setText(tr("下载结束，请等待小板保存完成..."));

                    return;
                }

            }
            downloadIndex++;

            //延时：CmdACKDelay
            Model_Delay delayDeal;
            delayDeal.usleep(CmdACKDelay);
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

QString ResHardware::recovCANBaudDeal(uint16_t baud)
{
    QString Dat_temp=NULL;
    if(baud==10)       Dat_temp="10K";
    else if(baud==20)  Dat_temp="20K";
    else if(baud==33)  Dat_temp="33.3K";
    else if(baud==40)  Dat_temp="40K";
    else if(baud==50)  Dat_temp="50K";
    else if(baud==80)  Dat_temp="80K";
    else if(baud==83)  Dat_temp="83.3K";
    else if(baud==100) Dat_temp="100K";
    else if(baud==125) Dat_temp="125K";
    else if(baud==200) Dat_temp="200K";
    else if(baud==250) Dat_temp="250K";
    else if(baud==400) Dat_temp="400K";
    else if(baud==500) Dat_temp="500K";
    else if(baud==800) Dat_temp="800K";
    else if(baud==1000)   Dat_temp="1M";
    return Dat_temp;
}

/*************************************************************
/函数功能：打开或关闭CAN1
/函数参数：checked  true :开  false:关
/函数返回：无
/帧结构：Byte0：开/关   Byte1：CAN类型    Byte2~3：CAN波特率
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


    uint16_t CANbaud = covCANBaudDeal(ui->comboBoxCAN1Baud->currentText());
    char buf[BUFSIZ]={0};

    buf[0] = checked;

    if(ui->comboBoxCAN1Type->currentIndex()==0)//类型
        buf[1]=CAN1CHANNEL;
    else
        buf[1]=CANSingle;

    buf[2]=CANbaud>>8;
    buf[3]=CANbaud&0x00ff;

    keyUart->appendTxList(CMDCANChannel,buf,4,CMD_NEEDACK);
    ui->label_Show->setText(tr("下载:CAN1通道配置"));
}

/*************************************************************
/函数功能：打开或关闭CAN2
/函数参数：checked  true :开  false:关
/函数返回：无
/帧结构：Byte0：开/关   Byte1：CAN类型    Byte2~3：CAN波特率
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

    uint16_t CANbaud = covCANBaudDeal(ui->comboBoxCAN2Baud->currentText());
    char buf[BUFSIZ]={0};

    buf[0] = checked;
    buf[1] = CAN2CHANNEL;
    buf[2]=CANbaud>>8;
    buf[3]=CANbaud&0x00ff;
    keyUart->appendTxList(CMDCANChannel,buf,4,CMD_NEEDACK);
    ui->label_Show->setText(tr("下载:CAN2通道配置"));
}

/*************************************************************
/函数功能：读CAN通道配置
/函数参数：wu
/函数返回：无
*************************************************************/
void ResHardware::uartReadCANParamDeal()
{
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        return ;
    }
    char read;
    keyUart->appendTxList(CMDUploadCAN,&read,1,CMD_NEEDNACK);
    ui->label_Show->setText(tr("读取：CAN参数配置"));
}

/*************************************************************
/函数功能：设置BAT电压值
/函数参数：值
/函数返回：无
/帧结构：Byte0：电压值
*************************************************************/
void ResHardware::on_horizontalSliderBAT_valueChanged(int value)
{
    ui->label_ShowBATVal->setText(toStr(value));
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        return ;
    }

    char buf[BUFSIZ]={0};

    buf[0] = value;
    keyUart->appendTxList(CMDBATPower,buf,1,CMD_NEEDACK);
    ui->label_Show->setText(tr("下载:Bat电压值")+toStr(value));
}

/*************************************************************
/函数功能：设置CCD电压值
/函数参数：值
/函数返回：无
/帧结构：Byte0：电压值
*************************************************************/
void ResHardware::on_horizontalSliderCCD_valueChanged(int value)
{
    ui->label_ShowCCDVal->setText(toStr(value));
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        return ;
    }

    char buf[BUFSIZ]={0};

    buf[0] = value;
    keyUart->appendTxList(CMDCCDPower,buf,1,CMD_NEEDACK);
    ui->label_Show->setText(tr("下载:摄像头电压值")+toStr(value));
}

/*************************************************************
/函数功能：设置15V最大电压值
/函数参数：无
/函数返回：无
/帧结构：Byte0：false：15V最大电压；true：24V最大电压
*************************************************************/
void ResHardware::on_radioBtn15V_clicked()
{
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        ui->radioBtn24V->setChecked(true);
        return ;
    }

    char buf[BUFSIZ]={0};

    buf[0] = false;
    keyUart->appendTxList(CMDBATMaxVal,buf,1,CMD_NEEDACK);
    ui->label_Show->setText(tr("下载:Bat最大工作电压"));

    ui->horizontalSliderBAT->setMaximum(15);
    ui->horizontalSliderCCD->setMaximum(15);
}

/*************************************************************
/函数功能：设置24V最大电压值
/函数参数：wu
/函数返回：无
/帧结构：Byte0：false：15V最大电压；true：24V最大电压
*************************************************************/
void ResHardware::on_radioBtn24V_clicked()
{
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        ui->radioBtn15V->setChecked(true);
        return ;
    }

    char buf[BUFSIZ]={0};

    buf[0] = true;
    keyUart->appendTxList(CMDBATMaxVal,buf,1,CMD_NEEDACK);
    ui->label_Show->setText(tr("下载:Bat最大工作电压"));

    ui->horizontalSliderBAT->setMaximum(24);
    ui->horizontalSliderCCD->setMaximum(24);
}

/*************************************************************
/函数功能：读取当前工作电压
/函数参数：wu
/函数返回：无
*************************************************************/
void ResHardware::on_pushBtnReadVB_clicked()
{
    if(keyUart->isOpenCurrentUart()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("未打开串口，无法进行下载操作！"));
        return ;
    }

    char buf[BUFSIZ]={0};

    buf[0] = true;
    keyUart->appendTxList(CMDUploadBatVal,buf,1,CMD_NEEDNACK);
    ui->label_Show->setText(tr("读取当前工作电压..."));
}

/*************************************************************
/函数功能：点击按键操作
/函数参数：无
/函数返回：无
/备注：手动点击按键，用来测试
*************************************************************/
void ResHardware::on_pushButton_KeyClicked_clicked()
{
    char buf[2]={0};
    int keyNum = getKeyNumber(ui->comboBoxKeyList->currentText());
    if(keyNum != -1)
    {
        buf[0] = keyNum;//KEY1  取"1"

        if(ui->checkBoxONOFF->checkState() == Qt::Checked)
            buf[1]=1;
        else
            buf[1]=0;
        keyUart->appendTxList(CMDClickedKey,buf,2,CMD_NEEDACK);
    }
}

void ResHardware::on_pushBtnHelp_clicked()
{
    QString pdfPath=QFileDialog::getOpenFileName(this , tr("选择项目文件") , "./config/Help" , tr("Text Files(*.pdf)"));//QDir::currentPath()+"/config/Help/ATtool使用说明.pdf";

    if(pdfPath.isEmpty()==false)
    {
        if(QDesktopServices::openUrl(QUrl::fromLocalFile(pdfPath))==false)
        {
            QMessageBox::warning(NULL, tr("提示"), tr("打开文件错误，请重试！"));
        }
    }
}
