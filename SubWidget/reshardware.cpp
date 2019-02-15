#include "reshardware.h"
#include "ui_reshardware.h"

ResHardware::ResHardware(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResHardware)
{
    ui->setupUi(this);

    QStringList itemList;
    readItemListInfo(itemList);
    ui->comboBox_itemName->addItems(itemList);

    //any:若是均改变将都处理，会重复处理
    connect(ui->comboBox_itemName,SIGNAL(activated(QString)),this,SLOT(itemNameSlot(QString)));
    connect(ui->comboBox_itemName,SIGNAL(currentTextChanged(QString)),this,SLOT(itemNameSlot(QString)));

    ui->comboBox_itemName->setCurrentText(WorkItem);

    keyList.clear();

    tableWidgetInit();

    if(ui->comboBox_itemName->currentText().isEmpty() == false)
    {
        readItemKeyInfo(ui->comboBox_itemName->currentText());
    }
}

ResHardware::~ResHardware()
{
    delete ui;
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
        keyControl *keyInfo=getkeyControlNull();

        //填写具体信息
        switch(i)
        {
        case 0:
            keyInfo->name = "ACC";
            keyInfo->type = HardACC;
            keyInfo->isUse = true;
            break;
        case 1:
            keyInfo->name = "BAT";
            keyInfo->type = HardBAT;
            keyInfo->isUse = true;
            break;
        case 2:
            keyInfo->name = "CCD";
            keyInfo->type = HardCCD;
            keyInfo->isUse = true;
            break;
        }

        keyInfo->des = "name : "+keyInfo->name;
        keyInfo->des +="\ntype : "+getKeyType(keyInfo->type);
        //替换显示及列表信息
        ui->tableWidget->item(i,colDes)->setText(keyInfo->des);
        //key:isEnable
        ui->tableWidget->item(i,colKey)->setCheckState(Qt::Checked);

        //按键信息添加到列表
        keyList.replace(i,*keyInfo);
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
        keyControl *keyInfo=getkeyControlNull();
        int row=ui->tableWidget->rowCount();
        ui->tableWidget->setRowCount(row+1);

        //key：isUse
        QTableWidgetItem *keyName=new QTableWidgetItem("KEY"+QString::number(row+1));
        if(keyInfo->isUse)
            keyName->setCheckState(Qt::Checked);
        else
            keyName->setCheckState(Qt::Unchecked);
        ui->tableWidget->setItem(row,colKey,keyName);

        //key :Description
        QTableWidgetItem *keyDes=new QTableWidgetItem(keyInfo->des);
        ui->tableWidget->setItem(row,colDes,keyDes);

        //key:Edit
        QPushButton *pBtn = new QPushButton(QIcon(":/test/edit.png"),"Edit");
        pBtn->setFlat(true);
        if(row>=fixedKeyNum)
            connect(pBtn, SIGNAL(clicked()), this, SLOT(EditKeyClicked()));
        ui->tableWidget->setCellWidget(row,colEdit,pBtn);

        //key:Width
        ui->tableWidget->setColumnWidth(colKey,80);
        ui->tableWidget->setColumnWidth(colDes,410);

        //key:isEnable
        keyisUsetoEnable(row,keyInfo->isUse);

        //按键信息添加到列表
        keyList.append(*keyInfo);
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

        if(theKey.type == Can1_1)
            theKey.des +="\nCANID : "+theKey.CANID+"\nCANDat : "+theKey.CANDat1;

        else if(theKey.type == Can1_2)
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

/*************************************************************
/函数功能：上传数据信息，从小板获取项目号，再从文件中读取该项目的按键内容
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::on_pushButton_inDat_clicked()
{
    ResUpAndDownLoads *UpLoads=new ResUpAndDownLoads(false,&keyList);

    UpLoads->exec();

    QString upItem = UpLoads->getCurrentItem();

    if((ui->comboBox_itemName->currentText() != upItem) && (!upItem.isEmpty()))
    {
        ui->comboBox_itemName->setCurrentText(upItem);
        if(ui->comboBox_itemName->currentText() != upItem)
            QMessageBox::warning(NULL, QString("Warn"), QString("未找到"+upItem+"项目的按键信息，请更新KeyInfo.xml文件或重新配置按键信息！"));
    }
    delete UpLoads;
}

/*************************************************************
/函数功能：下载并保存按键信息
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardware::on_pushButton_outDat_clicked()
{
    ResUpAndDownLoads *DownLoads=new ResUpAndDownLoads(true,&keyList);

    DownLoads->setdownItemName(ui->comboBox_itemName->currentText());
    DownLoads->exec();

    QString downItem = DownLoads->getCurrentItem();

    if( (ui->comboBox_itemName->findText(downItem) == -1) && (!downItem.isEmpty()) )
    {
        QStringList itemList;
        readItemListInfo(itemList);
        if(itemList.count()!=ui->comboBox_itemName->count())
            ui->comboBox_itemName->addItem(downItem);
        else
            cout << "该项目未保存到文件中";

    }
    ui->comboBox_itemName->setCurrentText(downItem);//若设置的字符串不存在将不跳转

    delete DownLoads;
}

/*************************************************************
/函数功能：切换项目编号，刷新项目列表：因此在需要的地方只需更改项目编号而已，无需再操心刷新问题
/函数参数：项目
/函数返回：无
*************************************************************/
void ResHardware::itemNameSlot(const QString &arg1)
{
    readItemKeyInfo(arg1);
    WorkItem=arg1;
    Model_iniSetting itemName;
    itemName.WriteIni_item(WorkItem);
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

