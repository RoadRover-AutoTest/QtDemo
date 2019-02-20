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
    initTabCCD();
}

CfgSParam::~CfgSParam()
{
    delete ui;
}

/*************************************************************
/函数功能：初始化检测参数
/函数参数：无
/函数返回：wu
*************************************************************/
void CfgSParam::initNullChkParam(checkParam *chkParam)
{
    chkParam->check =CHKRES;
    chkParam->range=GE;
    chkParam->max=0;
    chkParam->min=0;
    chkParam->hReault=0;
    chkParam->sound=endHAVESound;
    chkParam->infoCompare=NoCompare;
    chkParam->logContains.clear();
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

    int waitMin  = topItem->child(acc_Off_WaitTime)->child(acc_Off_Change_min)->text(acc_colItem).toUInt();
    int waitMax = topItem->child(acc_Off_WaitTime)->child(acc_Off_Change_max)->text(acc_colItem).toUInt();
    int waitStep = topItem->child(acc_Off_WaitTime)->child(acc_Off_Change_step)->text(acc_colItem).toUInt();

    offAction->actName = "ACCOFF";
    offAction->actStr =  getAccKey()+":off";

    offAction->timeDeal.wait = waitMin;
    offAction->timeDeal.check = 0;
    offAction->timeDeal.end = 60000;

    //添加变动参数：存在步进值，且最大和最小值不相等
    if((waitStep != 0) && (waitMax != waitMin))
    {
        changedParam changeTime;

        changeTime.changed = WaitTime;
        if(waitMax > waitMin)
            changeTime.dir = true;
        else
            changeTime.dir = false;
        changeTime.min  = waitMin;
        changeTime.max  = waitMax;
        changeTime.step = waitStep;

        offAction->changedDeal.append(changeTime);
    }

    //休眠检测：
    if(topItem->child(acc_Off_Check)->checkState(acc_colItem) == Qt::Checked)
    {
        checkParam checkSleep;

        initNullChkParam(&checkSleep);

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

    //电流检测：
    if(topItem->child(acc_On_CheckCurrent)->checkState(acc_colItem) == Qt::Checked)
    {
        checkParam checkCurrent;

        initNullChkParam(&checkCurrent);

        checkCurrent.check = CHKCurrent;
        checkCurrent.range = GE;
        checkCurrent.min = topItem->child(acc_On_CheckCurrent)->child(acc_On_CurrentValue)->text(acc_colItem).toUInt();

        onAction->checkDeal.append(checkCurrent);
    }

    //界面检测：
    if(topItem->child(acc_On_CheckMemory)->checkState(acc_colItem) == Qt::Checked)
    {
        checkParam checkMemoey;

        initNullChkParam(&checkMemoey);

        checkMemoey.check = CHKInterface;
        if(topItem->child(acc_On_CheckMemory)->child(acc_On_MemoryBool)->checkState(acc_colItem) == Qt::Checked)
            checkMemoey.infoCompare = MemoryCompare;
        else
            checkMemoey.infoCompare = NoCompare;
        onAction->checkDeal.append(checkMemoey);
    }
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

    initNullChkParam(&checkScript);

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

    int waitMin  = topItem->child(bat_Off_WaitTime)->child(bat_Off_ChangeMin)->text(acc_colItem).toUInt();
    int waitMax = topItem->child(bat_Off_WaitTime)->child(bat_Off_ChangeMax)->text(acc_colItem).toUInt();
    int waitStep = topItem->child(bat_Off_WaitTime)->child(bat_Off_ChangeStep)->text(acc_colItem).toUInt();

    offAction->actName = "BATOFF";
    offAction->actStr =  getBatKey()+":off";

    offAction->timeDeal.wait = waitMin;
    offAction->timeDeal.check = 0;
    offAction->timeDeal.end = 60000;

    //添加变动参数：存在步进值，且最大和最小值不相等
    if((waitStep != 0) && (waitMax != waitMin))
    {
        changedParam changeTime;

        changeTime.changed = WaitTime;
        if(waitMax > waitMin)
            changeTime.dir = true;
        else
            changeTime.dir = false;
        changeTime.min  = waitMin;
        changeTime.max  = waitMax;
        changeTime.step = waitStep;

        offAction->changedDeal.append(changeTime);
    }

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

    //电流检测：
    if(topItem->child(bat_On_CheckCurrent)->checkState(bat_colItem) == Qt::Checked)
    {
        checkParam checkCurrent;

        initNullChkParam(&checkCurrent);

        checkCurrent.check = CHKCurrent;
        checkCurrent.range = GE;
        checkCurrent.min = topItem->child(bat_On_CheckCurrent)->child(bat_On_CurrentValue)->text(bat_colItem).toUInt();
        onAction->checkDeal.append(checkCurrent);
    }

    //界面检测：
    if(topItem->child(bat_On_CheckMemory)->checkState(bat_colItem) == Qt::Checked)
    {
        checkParam checkMemoey;
        initNullChkParam(&checkMemoey);

        checkMemoey.check = CHKInterface;
        if(topItem->child(bat_On_CheckMemory)->child(bat_On_MemoryBool)->checkState(bat_colItem) == Qt::Checked)
            checkMemoey.infoCompare = MemoryCompare;
        else
            checkMemoey.infoCompare = NoCompare;
        onAction->checkDeal.append(checkMemoey);
    }
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
    initNullChkParam(&checkScript);

    checkScript.check = CHKScript;
    checkScript.logContains = topItem->child(bat_Script_log)->text(bat_colItem);
    scriptAction->checkDeal.append(checkScript);
}


/*************************************************************
/函数功能：初始化CCD页
/函数参数：无
/函数返回：wu
*************************************************************/
void CfgSParam::initTabCCD()
{
    //填充CCD列表
    QComboBox *CCDList = new QComboBox;

    for(int i=0;i<keyList.length();i++)
    {
        CCDList->addItem("KEY"+QString::number(i+1)+":"+keyList.at(i).name);
    }

    ui->treeCCD->setItemWidget(ui->treeCCD->topLevelItem(ccd_topKey),ccd_colItem,CCDList);

    //tree格式设置
    ui->treeCCD->setColumnWidth(0,200);
    ui->treeCCD->expandAll();
}

/*************************************************************
/函数功能：获取CCD选择的按键字符串
/函数参数：无
/函数返回：按键
*************************************************************/
QString CfgSParam::getCCDKey()
{
    QComboBox *comboBox=(QComboBox *)ui->treeCCD->itemWidget(ui->treeCCD->topLevelItem(ccd_topKey),ccd_colItem);
    return comboBox->currentText();
}

/*************************************************************
/函数功能：创建CCDoff动作数据
/函数参数：off动作
/函数返回：无
*************************************************************/
void CfgSParam::createCCDOffAction(tAction *offAction)
{
    QTreeWidgetItem *topItem = ui->treeCCD->topLevelItem(ccd_topOff);

    offAction->actName = "CCDOFF";
    offAction->actStr =  getCCDKey()+":off";

    offAction->timeDeal.wait = topItem->child(ccd_Off_WaitTime)->text(ccd_colItem).toUInt();
    offAction->timeDeal.check = 0;
    offAction->timeDeal.end = 60000;

    //界面检测：
    if(topItem->child(ccd_Off_CheckFace)->checkState(ccd_colItem) == Qt::Checked)
    {
        checkParam checkMemoey;
        initNullChkParam(&checkMemoey);
        checkMemoey.check = CHKInterface;
        checkMemoey.infoCompare = MemoryCompare;
        offAction->checkDeal.append(checkMemoey);
    }

    //声音检测：
    if(topItem->child(ccd_Off_CheckSound)->checkState(ccd_colItem) == Qt::Checked)
    {
        checkParam checkSound;
        initNullChkParam(&checkSound);
        checkSound.check = CHKSound;
        checkSound.sound = endHAVESound;
        offAction->checkDeal.append(checkSound);
    }
}

/*************************************************************
/函数功能：创建CCDon动作数据
/函数参数：on动作
/函数返回：无
*************************************************************/
void CfgSParam::createCCDOnAction(tAction *onAction)
{
    QTreeWidgetItem *topItem = ui->treeCCD->topLevelItem(ccd_topOn);

    int waitMin  = topItem->child(ccd_On_Waittime)->child(ccd_On_ChangeMin)->text(ccd_colItem).toUInt();
    int waitMax = topItem->child(ccd_On_Waittime)->child(ccd_On_ChangeMax)->text(ccd_colItem).toUInt();
    int waitStep = topItem->child(ccd_On_Waittime)->child(ccd_On_ChangeStep)->text(ccd_colItem).toUInt();

    onAction->actName = "CCDON";
    onAction->actStr =  getCCDKey()+":on";

    onAction->timeDeal.wait = waitMin;
    onAction->timeDeal.check = 0;
    onAction->timeDeal.end = 60000;

    //添加变动参数：存在步进值，且最大和最小值不相等
    if((waitStep != 0) && (waitMax != waitMin))
    {
        changedParam changeTime;

        changeTime.changed = WaitTime;
        if(waitMax > waitMin)
            changeTime.dir = true;
        else
            changeTime.dir = false;
        changeTime.min  = waitMin;
        changeTime.max  = waitMax;
        changeTime.step = waitStep;

        onAction->changedDeal.append(changeTime);
    }


    //图像识别：
    if(topItem->child(ccd_On_CheckPic)->checkState(ccd_colItem) == Qt::Checked)
    {
        checkParam checkPicture;
        initNullChkParam(&checkPicture);
        checkPicture.check = CHKADBPIC;
        checkPicture.infoCompare = SelfCompare;
        onAction->checkDeal.append(checkPicture);
    }
}

/*************************************************************
/函数功能：创建脚本动作数据
/函数参数：脚本动作
/函数返回：无
*************************************************************/
void CfgSParam::createCCDScriptAction(tAction *scriptAction)
{
    QTreeWidgetItem *topItem = ui->treeCCD->topLevelItem(ccd_topScript);

    //scriptAction->actName = "ACCON";
    //scriptAction->actStr =  getAccKey()+":on";//在函数外处理，具体使用处添加

    scriptAction->timeDeal.wait = 0;
    scriptAction->timeDeal.check = 0;
    scriptAction->timeDeal.end = 0;

    checkParam checkScript;
    initNullChkParam(&checkScript);

    checkScript.check = CHKScript;
    checkScript.logContains = topItem->child(ccd_Script_log)->text(ccd_colItem);
    scriptAction->checkDeal.append(checkScript);
}


/*************************************************************
/函数功能：将动作添加到测试单元中
/函数参数：unit   act
/函数返回：wu
*************************************************************/
void CfgSParam::addTheUnit(tUnit *unit,tAction *act)
{
    //根据检测项目，添加采集信息
    act->infoFlag = 0;
    for(int i=0;i<act->checkDeal.length();i++)
    {
        checkParam chkDat=act->checkDeal.at(i);
        switch(chkDat.check)
        {
        case CHKCurrent:
        act->infoFlag |= COLCURRENT;
        break;
        case CHKSound:
        act->infoFlag |= COLSOUND;
        break;
        //记忆检测：界面检测
        case CHKInterface:
        {
            act->infoFlag |= COLFACE;
            act->infoFlag |= COLFACESITE;//动作执行后采集
            goto ADDINFODAT;
            break;
        }
        //图片检测：
        case CHKADBPIC:
        {
            act->infoFlag |= COLPICTURE;
            act->infoFlag |= COLPICTURESITE;//动作执行后采集

            if(chkDat.infoCompare != MemoryCompare)
                break;//非记忆比较，均无需对应动作获取信息

            //非比较首次图片，即与测试过程中其他图片比较
            ADDINFODAT:
            bool curStatus ;

            if(act->actStr.contains(":on"))
                curStatus=true;
            else
                curStatus=false;

            //同时查找该动作之前测试单元中前一个状态下 添加上动作执行前采集当前界面信息
            for(int j=0;j<unit->actTest.length();j++)
            {
                QString actStr=act->actStr;
                QString unitfindStr = unit->actTest.at(j).actStr;

                if(((curStatus)&&((unitfindStr.contains(":off"))&&(unitfindStr.contains(actStr.remove(":on")))))
                 ||((!curStatus)&&((unitfindStr.contains(":on"))&&(unitfindStr.contains(actStr.remove(":off"))))))
                {
                    tAction findAction = unit->actTest.at(j);

                    findAction.infoFlag |= COLFACE;
                    if(chkDat.check == CHKInterface)
                        findAction.infoFlag &= ~(1<<5);//COLFACESITE
                    else if(chkDat.check == CHKADBPIC)
                        findAction.infoFlag &= ~(1<<7);//COLPICTURESITE

                    unit->actTest.replace(j,findAction);
                }
            }
            break;
        }
        default:break;
        }
    }

    //将测试动作添加到测试单元中去：
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

            if(ui->groupCCD->isChecked())
            {
                tUnit theUnit;
                tAction theAction;
                QString Script = filename.split('/').last().remove(".bat");


                theUnit.name = getCCDKey().split(":").last()+"_"+Script;
                theUnit.cycleCount = ui->spinCCDCycle->value();
                theUnit.unitDes = ui->textCCDDescription->toPlainText();

                createCCDOnAction(&theAction);
                addTheUnit(&theUnit,&theAction);

                createCCDOffAction(&theAction);
                addTheUnit(&theUnit,&theAction);

                theAction.actName = Script;
                theAction.actStr = filename;
                createCCDScriptAction(&theAction);
                addTheUnit(&theUnit,&theAction);

                listUnit->append(theUnit);
            }
        }
    }
    QDialog::accept();
}
