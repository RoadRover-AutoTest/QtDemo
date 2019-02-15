#include "cfgsparam.h"
#include "ui_cfgsparam.h"

CfgSParam::CfgSParam(QList <tUnit> *thelist,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CfgSParam)
{
    ui->setupUi(this);
    ui->tabWidget->tabBar()->setStyle(new CustomTabStyle);

    listUnit = thelist;

    initKeyList();

    initTabACC();
    initTabBAT();
}

CfgSParam::~CfgSParam()
{
    delete ui;
}

/*************************************************************
/函数功能：初始化按键选择
/函数参数：无
/函数返回：wu
*************************************************************/
void CfgSParam::initKeyList()
{
    Model_XMLFile xmlRead;
    QStringList showList;

    xmlRead.readKeyInfoXML(WorkItem,&keyList);

    if(keyList.isEmpty()==false)
    {
        for(int i=0;i<keyList.length();i++)
        {
            if(keyList.at(i).isUse)
            {
                showList.append("KEY"+QString::number(i+1)+":"+keyList.at(i).name);
            }
        }
    }
}

/*************************************************************
/函数功能：初始化ACC页
/函数参数：无
/函数返回：wu
*************************************************************/
void CfgSParam::initTabACC()
{
    //填充ACC列表
    QComboBox *AccList = new QComboBox;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardACC))
            AccList->addItem("KEY"+QString::number(i+1)+":"+keyList.at(i).name);
    }

    ui->treeACC->setItemWidget(ui->treeACC->topLevelItem(acc_topKey),acc_colItem,AccList);

    //tree格式设置
    ui->treeACC->setColumnWidth(0,200);
    ui->treeACC->expandAll();

}

/*************************************************************
/函数功能：获取ACC选择的按键字符串
/函数参数：无
/函数返回：按键
*************************************************************/
QString CfgSParam::getAccKey()
{
    QComboBox *comboBox=(QComboBox *)ui->treeACC->itemWidget(ui->treeACC->topLevelItem(acc_topKey),acc_colItem);
    return comboBox->currentText();
}

/*************************************************************
/函数功能：创建ACCoff动作数据
/函数参数：off动作
/函数返回：无
*************************************************************/
void CfgSParam::createACCOffAction(tAction *offAction)
{
    QTreeWidgetItem *topItem = ui->treeACC->topLevelItem(acc_topOff);

    offAction->actName = "ACCOFF";
    offAction->actStr =  getAccKey()+":off";

    offAction->timeDeal.wait = topItem->child(acc_Off_WaitTime)->text(acc_colItem).toUInt();
    offAction->timeDeal.check = 0;
    offAction->timeDeal.end = 60000;

    if(topItem->child(acc_Off_TimeChange)->checkState(acc_colItem) == Qt::Checked)
    {
        changedParam changeTime;

        changeTime.changed = WaitTime;
        if(topItem->child(acc_Off_TimeChange)->child(acc_Off_Change_dir)->checkState(acc_colItem) == Qt::Checked)
            changeTime.dir = true;
        else
            changeTime.dir = false;
        changeTime.min = topItem->child(acc_Off_TimeChange)->child(acc_Off_Change_min)->text(acc_colItem).toUInt();
        changeTime.max = topItem->child(acc_Off_TimeChange)->child(acc_Off_Change_max)->text(acc_colItem).toUInt();
        changeTime.step = topItem->child(acc_Off_TimeChange)->child(acc_Off_Change_step)->text(acc_colItem).toUInt();

        offAction->changedDeal.append(changeTime);
    }

    if(topItem->child(acc_Off_Check)->checkState(acc_colItem) == Qt::Checked)
    {
        checkParam checkSleep;

        checkSleep.check = CHKCurrent;
        checkSleep.range = LE;
        checkSleep.min = topItem->child(acc_Off_Check)->child(acc_Off_Check_Current)->text(acc_colItem).toUInt();

        offAction->timeDeal.check = topItem->child(acc_Off_Check)->child(acc_Off_Check_time)->text(acc_colItem).toUInt();
        offAction->checkDeal.append(checkSleep);
    }
}

/*************************************************************
/函数功能：创建ACCon动作数据
/函数参数：on动作
/函数返回：无
*************************************************************/
void CfgSParam::createACCOnAction(tAction *onAction)
{
    QTreeWidgetItem *topItem = ui->treeACC->topLevelItem(acc_topOn);

    onAction->actName = "ACCON";
    onAction->actStr =  getAccKey()+":on";

    onAction->timeDeal.wait = topItem->child(acc_On_WaitTime)->text(acc_colItem).toUInt();
    onAction->timeDeal.check = 0;
    onAction->timeDeal.end = topItem->child(acc_On_EndTime)->text(acc_colItem).toUInt();

    if(topItem->child(acc_On_CheckCurrent)->checkState(acc_colItem) == Qt::Checked)
    {
        checkParam checkCurrent;

        checkCurrent.check = CHKCurrent;
        checkCurrent.range = GE;
        checkCurrent.min = topItem->child(acc_On_CheckCurrent)->child(acc_On_CurrentValue)->text(acc_colItem).toUInt();
        onAction->checkDeal.append(checkCurrent);
    }

    checkParam checkMemoey;

    checkMemoey.check = CHKMEMORY;
    if(topItem->child(acc_On_CheckMemory)->checkState(acc_colItem) == Qt::Checked)
        checkMemoey.isMemory = true;
    else
        checkMemoey.isMemory = false;
    onAction->checkDeal.append(checkMemoey);

}

/*************************************************************
/函数功能：创建脚本动作数据
/函数参数：脚本动作
/函数返回：无
*************************************************************/
void CfgSParam::createScriptAction(tAction *scriptAction)
{
    QTreeWidgetItem *topItem = ui->treeACC->topLevelItem(acc_topScript);

    //scriptAction->actName = "ACCON";
    //scriptAction->actStr =  getAccKey()+":on";//在函数外处理，具体使用处添加

    scriptAction->timeDeal.wait = 0;
    scriptAction->timeDeal.check = 0;
    scriptAction->timeDeal.end = 0;

    checkParam checkScript;

    checkScript.check = CHKScript;
    checkScript.logContains = topItem->child(acc_Script_log)->text(acc_colItem);
    scriptAction->checkDeal.append(checkScript);
}

/*************************************************************
/函数功能：初始化BAT页
/函数参数：无
/函数返回：wu
*************************************************************/
void CfgSParam::initTabBAT()
{
    //填充ACC列表
    QComboBox *BatList = new QComboBox;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardBAT))
            BatList->addItem("KEY"+QString::number(i+1)+":"+keyList.at(i).name);
    }

    ui->treeBAT->setItemWidget(ui->treeBAT->topLevelItem(bat_topKey),bat_colItem,BatList);

    //tree格式设置
    ui->treeBAT->setColumnWidth(0,200);
    ui->treeBAT->expandAll();

}

/*************************************************************
/函数功能：获取Bat选择的按键字符串
/函数参数：无
/函数返回：按键
*************************************************************/
QString CfgSParam::getBatKey()
{
    QComboBox *comboBox=(QComboBox *)ui->treeBAT->itemWidget(ui->treeBAT->topLevelItem(bat_topKey),bat_colItem);
    return comboBox->currentText();
}

/*************************************************************
/函数功能：创建BAToff动作数据
/函数参数：off动作
/函数返回：无
*************************************************************/
void CfgSParam::createBatOffAction(tAction *offAction)
{
    QTreeWidgetItem *topItem = ui->treeBAT->topLevelItem(bat_topOff);

    offAction->actName = "BATOFF";
    offAction->actStr =  getBatKey()+":off";

    offAction->timeDeal.wait = topItem->child(bat_Off_WaitTime)->text(bat_colItem).toUInt();
    offAction->timeDeal.check = 0;
    offAction->timeDeal.end = 60000;
}

/*************************************************************
/函数功能：创建BATon动作数据
/函数参数：on动作
/函数返回：无
*************************************************************/
void CfgSParam::createBATOnAction(tAction *onAction)
{
    QTreeWidgetItem *topItem = ui->treeBAT->topLevelItem(bat_topOn);

    onAction->actName = "BATON";
    onAction->actStr =  getBatKey()+":on";

    onAction->timeDeal.wait = topItem->child(bat_On_Waittime)->text(bat_colItem).toUInt();
    onAction->timeDeal.check = 0;
    onAction->timeDeal.end = topItem->child(bat_On_EndTime)->text(bat_colItem).toUInt();

    if(topItem->child(bat_On_CheckCurrent)->checkState(bat_colItem) == Qt::Checked)
    {
        checkParam checkCurrent;

        checkCurrent.check = CHKCurrent;
        checkCurrent.range = GE;
        checkCurrent.min = topItem->child(bat_On_CheckCurrent)->child(bat_On_CurrentValue)->text(bat_colItem).toUInt();
        onAction->checkDeal.append(checkCurrent);
    }

    checkParam checkMemoey;

    checkMemoey.check = CHKMEMORY;
    if(topItem->child(bat_On_CheckMemory)->checkState(bat_colItem) == Qt::Checked)
        checkMemoey.isMemory = true;
    else
        checkMemoey.isMemory = false;
    onAction->checkDeal.append(checkMemoey);

}

/*************************************************************
/函数功能：创建脚本动作数据
/函数参数：脚本动作
/函数返回：无
*************************************************************/
void CfgSParam::createBatScriptAction(tAction *scriptAction)
{
    QTreeWidgetItem *topItem = ui->treeBAT->topLevelItem(bat_topScript);

    //scriptAction->actName = "ACCON";
    //scriptAction->actStr =  getAccKey()+":on";//在函数外处理，具体使用处添加

    scriptAction->timeDeal.wait = 0;
    scriptAction->timeDeal.check = 0;
    scriptAction->timeDeal.end = 0;

    checkParam checkScript;

    checkScript.check = CHKScript;
    checkScript.logContains = topItem->child(bat_Script_log)->text(bat_colItem);
    scriptAction->checkDeal.append(checkScript);
}

/*************************************************************
/函数功能：将动作添加到测试单元中
/函数参数：unit   act
/函数返回：wu
*************************************************************/
void CfgSParam::addTheUnit(tUnit *unit,tAction *act)
{
    unit->actTest.append(*act);
    act->changedDeal.clear();
    act->checkDeal.clear();
}

/*************************************************************
/函数功能：点击选择脚本文件
/函数参数：无
/函数返回：无
*************************************************************/
void CfgSParam::on_toolopenFile_clicked()
{
    //选择脚本文件夹
    QString dirPath=QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("选择脚本文件"),QDir::currentPath()));

    if(!dirPath.isEmpty())
        ui->lineEditFilePath->setText(dirPath);
}

/*************************************************************
/函数功能：点击确认按键，创建动作链接
/函数参数：无
/函数返回：无
*************************************************************/
void CfgSParam::accept()
{
    QString dirPath = ui->lineEditFilePath->text();
    //判断路径是否存在
    QDir dir(dirPath);
    if((!dir.exists())||(dirPath.isEmpty()))
    {
        QMessageBox::warning(NULL, QString("Warn"), QString("未选择脚本文件或不存在该路径！"));
        return;
    }
    else
        dir.setFilter(QDir::Files | QDir::NoSymLinks);

    QFileInfoList list = dir.entryInfoList();

    for(int i=0;i<list.length();i++)
    {
        QString filename=list.at(i).absoluteFilePath();//.baseName();//QFileInfo

        //只取bat文件
        if(filename.contains(".bat") || filename.contains(".BAT"))
        {
            if(ui->groupACC->isChecked())
            {
                tUnit theUnit;
                tAction theAction;
                QString Script = filename.split('/').last().remove(".bat");


                theUnit.name = getAccKey().split(":").last()+"_"+Script;
                theUnit.cycleCount = ui->spinAccCycle->value();
                theUnit.unitDes = ui->textACCDescription->toPlainText();

                createACCOffAction(&theAction);
                addTheUnit(&theUnit,&theAction);


                createACCOnAction(&theAction);
                addTheUnit(&theUnit,&theAction);

                theAction.actName = Script;
                theAction.actStr = filename;
                createScriptAction(&theAction);
                addTheUnit(&theUnit,&theAction);

                listUnit->append(theUnit);

            }


            if(ui->groupBAT->isChecked())
            {
                tUnit theUnit;
                tAction theAction;
                QString Script = filename.split('/').last().remove(".bat");


                theUnit.name = getBatKey().split(":").last()+"_"+Script;
                theUnit.cycleCount = ui->spinBatCycle->value();
                theUnit.unitDes = ui->textBatDescription->toPlainText();

                createBatOffAction(&theAction);
                addTheUnit(&theUnit,&theAction);


                createBATOnAction(&theAction);
                addTheUnit(&theUnit,&theAction);

                theAction.actName = Script;
                theAction.actStr = filename;
                createBatScriptAction(&theAction);
                addTheUnit(&theUnit,&theAction);

                listUnit->append(theUnit);
            }
        }
    }
    QDialog::accept();
}