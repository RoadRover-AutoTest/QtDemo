#include "reshardedit.h"
#include "ui_reshardedit.h"

ResHardEdit::ResHardEdit(keyControl *keyInfo,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResHardEdit)
{
    ui->setupUi(this);

    editKeyInfo=keyInfo;

    ui->stackedWidget->setCurrentIndex(0);

    keyEditInit();

    connect(ui->lineEditCAN1ID,SIGNAL(editingFinished()),this,SLOT(on_CAN1Changed()));
    connect(ui->lineEditCAN1Dat,SIGNAL(editingFinished()),this,SLOT(on_CAN1Changed()));

    connect(ui->lineEditCAN2ID,SIGNAL(editingFinished()),this,SLOT(on_CAN2Changed()));
    connect(ui->lineEditCAN2DatOn,SIGNAL(editingFinished()),this,SLOT(on_CAN2Changed()));
    connect(ui->lineEditCAN2DatOff,SIGNAL(editingFinished()),this,SLOT(on_CAN2Changed()));


    connect(ui->radioButtonACC,SIGNAL(clicked(bool)),this,SLOT(on_KeyTypeChanged()));
    connect(ui->radioButtonBAT,SIGNAL(clicked(bool)),this,SLOT(on_KeyTypeChanged()));
    connect(ui->radioButtonCCD,SIGNAL(clicked(bool)),this,SLOT(on_KeyTypeChanged()));
    connect(ui->radioButtonLAMP,SIGNAL(clicked(bool)),this,SLOT(on_KeyTypeChanged()));
    connect(ui->radioButtonRES,SIGNAL(clicked(bool)),this,SLOT(on_KeyTypeChanged()));
}

ResHardEdit::~ResHardEdit()
{
    delete ui;
}

/*************************************************************
/函数功能：根据传递过来的按键信息，初始化组件参数
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardEdit::keyEditInit()
{
    ui->lineEdit->setText(editKeyInfo->name);

    switch(editKeyInfo->type)
    {
    case HardACC:
        ui->radioButtonACC->setChecked(true);
        goto toHardPage;
    case HardBAT:
        ui->radioButtonBAT->setChecked(true);
        goto toHardPage;
    case HardCCD:
        ui->radioButtonCCD->setChecked(true);
        goto toHardPage;
    case HardLamp:
        ui->radioButtonLAMP->setChecked(true);
        goto toHardPage;
    case HardRes:
        ui->radioButtonRES->setChecked(true);
        goto toHardPage;


toHardPage:
        ui->comboBox->setCurrentIndex(0);
        break;
    case Can1_1:
        ui->comboBox->setCurrentIndex(1);
        ui->lineEditCAN1ID->setText(editKeyInfo->CANID);
        ui->lineEditCAN1Dat->setText(editKeyInfo->CANDat1);
        break;
    case Can1_2:
        ui->comboBox->setCurrentIndex(2);
        ui->lineEditCAN2ID->setText(editKeyInfo->CANID);
        ui->lineEditCAN2DatOn->setText(editKeyInfo->CANDat1);
        ui->lineEditCAN2DatOff->setText(editKeyInfo->CANDat2);
        break;
    default:break;
    }

}

/*************************************************************
/函数功能：测试类型更改时，切换页面，同步数据到按键信息指针
/函数参数：选择的类型指针：0：硬件 1：单协议 2：双协议
/函数返回：无
*************************************************************/
void ResHardEdit::on_comboBox_currentIndexChanged(int index)
{
    ui->stackedWidget->setCurrentIndex(index);
    showlabel();

    if(index)
    {
        editKeyInfo->type = (kType) (index - 1 + (int) Can1_1);
    }
    else
    {
        on_KeyTypeChanged();
    }
}

/*************************************************************
/函数功能：根据测试类型，更新信息提示
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardEdit::showlabel()
{
    switch (ui->stackedWidget->currentIndex())
    {
    case 0:
        ui->label->setText("硬件控制车机！");
        break;
    case 1:
        ui->label->setText("单协议：单帧协议控制车机");
        break;
    case 2:
        ui->label->setText("双协议：双帧控制车机，常用来处理翻转操作");
        break;
    default:
        break;
    }
}

/*************************************************************
/函数功能：修改按键名
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardEdit::on_lineEdit_editingFinished()
{
    editKeyInfo->name=ui->lineEdit->text();
}

/*************************************************************
/函数功能：修改单协议
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardEdit::on_CAN1Changed()
{
    QObject *sender = QObject::sender();

    if(sender==ui->lineEditCAN1ID)
        editKeyInfo->CANID  =  ui->lineEditCAN1ID->text();
    else if(sender==ui->lineEditCAN1Dat)
        editKeyInfo->CANDat1  =  ui->lineEditCAN1Dat->text();
}

/*************************************************************
/函数功能：修改双协议
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardEdit::on_CAN2Changed()
{
    QObject *sender = QObject::sender();

    if(sender==ui->lineEditCAN2ID)
        editKeyInfo->CANID  =  ui->lineEditCAN2ID->text();
    else if(sender==ui->lineEditCAN2DatOn)
        editKeyInfo->CANDat1  =  ui->lineEditCAN2DatOn->text();
    else if(sender==ui->lineEditCAN2DatOff)
        editKeyInfo->CANDat2  =  ui->lineEditCAN2DatOff->text();
}

/*************************************************************
/函数功能：修改硬件测试类型
/函数参数：无
/函数返回：无
*************************************************************/
void ResHardEdit::on_KeyTypeChanged()
{
    if(ui->radioButtonACC->isChecked())
        editKeyInfo->type = HardACC;
    if(ui->radioButtonBAT->isChecked())
        editKeyInfo->type = HardBAT;
    if(ui->radioButtonCCD->isChecked())
        editKeyInfo->type = HardCCD;
    if(ui->radioButtonLAMP->isChecked())
        editKeyInfo->type = HardLamp;
    if(ui->radioButtonRES->isChecked())
        editKeyInfo->type = HardRes;
}