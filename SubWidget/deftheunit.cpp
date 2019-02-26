#include "deftheunit.h"
#include "ui_deftheunit.h"

defTheUnit::defTheUnit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::defTheUnit)
{
    ui->setupUi(this);

    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //tab标签栏优化
    ui->tabProperties->tabBar()->setStyle(new CustomTabStyle);
    //ui->tabChkParam->tabBar()->setStyle(new CustomTabStyle);

    unitDeal.name = "";
    unitDeal.cycleCount = 1;
    unitDeal.unitDes = "";

    //初始化按键列表
    Model_XMLFile xmlRead;
    xmlRead.readKeyInfoXML(WorkItem,&keyList);

    for(int i=0;i<keyList.length();i++)
    {
        if(keyList.at(i).isUse)
            ui->comboKeyList->addItem("KEY"+QString::number(i+1)+":"+keyList.at(i).name);
    }
    ui->comboKeyList->setCurrentIndex(-1);

    connect(ui->editWaitMin,SIGNAL(editingFinished()),this,SLOT(editTimeDealSlot()));
    connect(ui->editWaitMax,SIGNAL(editingFinished()),this,SLOT(editTimeDealSlot()));
    connect(ui->editWaitStep,SIGNAL(editingFinished()),this,SLOT(editTimeDealSlot()));
    connect(ui->editChkTime,SIGNAL(editingFinished()),this,SLOT(editTimeDealSlot()));
    connect(ui->editOverTime,SIGNAL(editingFinished()),this,SLOT(editTimeDealSlot()));

    connect(ui->checkSetCurrent,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));
    connect(ui->checkSetVolt,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));
    connect(ui->checkSetSound,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));
    connect(ui->checkSetLogStr,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));
    connect(ui->checkSetInterface,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));
    connect(ui->checkSetPic,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));
    //connect(ui->checkSetCurrent,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));
    //connect(ui->checkSetCurrent,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));

}

defTheUnit::~defTheUnit()
{
    delete ui;
}

/*************************************************************
/函数功能：tableAction右键创建菜单栏
/函数参数：位置
/函数返回：wu
*************************************************************/
void defTheUnit::on_tableAction_customContextMenuRequested(const QPoint &pos)
{
    QMenu *popMenu = new QMenu( this );
    QMenu *addMenu = new QMenu("Add Action");
    QAction *keyAction = new QAction(tr("KEY"), this);
    QAction *ScriptAction = new QAction(tr("Script"), this);
    addMenu->addAction(keyAction);
    addMenu->addAction(ScriptAction);
    connect( keyAction,        SIGNAL(triggered() ), this, SLOT( keyActionSlot()) );
    connect( ScriptAction,        SIGNAL(triggered() ), this, SLOT( scriptActionSlot()) );


    popMenu->addMenu(addMenu);

    popMenu->exec( QCursor::pos() );

    delete popMenu;

}

void defTheUnit::on_editUnitName_editingFinished()
{
    unitDeal.name = ui->editUnitName->text();
}

void defTheUnit::on_spinUnitCycle_editingFinished()
{
    unitDeal.cycleCount = ui->spinUnitCycle->value();
}

void defTheUnit::on_editUnitDes_destroyed()
{
    //unitDeal.unitDes = ui->editUnitDes
}


void defTheUnit::inittActionParam(tAction *tact)
{
    tact->actName = "";
    tact->actStr =  "";
    tact->timeDeal.wait = 0;
    tact->timeDeal.check = 0;
    tact->timeDeal.end = 0;

    tact->infoFlag=0;
    tact->checkDeal.clear();
    tact->changedDeal.clear();
}

/*************************************************************
/函数功能：按键动作添加槽函数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::keyActionSlot()
{
    tAction kAction;
    inittActionParam(&kAction);
    kAction.actName = "KEY";
    kAction.actFlag = ACT_KEY;

    appendTableAction(kAction);
}

/*************************************************************
/函数功能：脚本动作添加槽函数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::scriptActionSlot()
{
    tAction sAction;
    inittActionParam(&sAction);

    sAction.actName="Script";
    sAction.actFlag = ACT_SCRIPT;

    //创建脚本检测
    checkParam checkScript;
    initNullChkParam(&checkScript);
    checkScript.check = CHKScript;
    checkScript.logContains = "OK (1 test)";
    sAction.checkDeal.append(checkScript);

    appendTableAction(sAction);
}

/*************************************************************
/函数功能：将动作添加到table中
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::appendTableAction(tAction act)
{
    int row=ui->tableAction->rowCount();
    ui->tableAction->setRowCount(row+1);

    ui->tableAction->setItem(row,Col_Name,new QTableWidgetItem(act.actName));
    ui->tableAction->setItem(row,Col_Str,new QTableWidgetItem(act.actStr));
    ui->tableAction->selectRow(row);

    unitDeal.actTest.append(act);
    refreshPropertiesParam(act);
}

/*************************************************************
/函数功能：点击table表格，刷新属性信息
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::on_tableAction_clicked(const QModelIndex &index)
{
    if(index.row()>=unitDeal.actTest.length())
        return ;

    tAction selAction = unitDeal.actTest.at(index.row());

    //刷新属性
    refreshPropertiesParam(selAction);
}

/*************************************************************
/函数功能：取table选择行号
/函数参数：无
/函数返回：wu
*************************************************************/
int defTheUnit::getTableActionSelRanges()
{
    int selrow;
    //因设置了单选，因此只取一个参数即可
    QList <QTableWidgetSelectionRange>ranges = ui->tableAction->selectedRanges();

    if(ranges.isEmpty()==false)
        selrow = ranges.at(0).topRow();
    return selrow;
}

/*************************************************************
/函数功能：刷新属性信息
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::refreshPropertiesParam(tAction act)
{
    ui->editActName->setText(act.actName);

    //刷新动作属性
    if(act.actFlag == ACT_KEY)
    {
        ui->stackedWidget->setCurrentWidget(ui->pageKey);
        refreshKeyList(act.actStr);
    }
    else if(act.actFlag == ACT_SCRIPT)
    {
        ui->stackedWidget->setCurrentWidget(ui->pageScript);
        ui->editFilePath->setText(act.actStr);
    }

    //刷新时间属性
    refreshTimeDeal(act);
    //刷新检测属性
    refreshCheckDeal(act.checkDeal);
}

/*************************************************************
/函数功能：表格内编辑动作
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::on_tableAction_itemChanged(QTableWidgetItem *item)
{
    int colNum = item->column();
    int selRow = item->row();

    if(selRow>=unitDeal.actTest.length())
        return ;

    if(colNum == Col_Name)
    {
        //修改列表信息
        tAction curAct = unitDeal.actTest.at(selRow);
        curAct.actName = item->text();
        unitDeal.actTest.replace(selRow,curAct);

        ui->editActName->setText(curAct.actName);
    }
    else if(colNum == Col_Str)
    {
        //修改列表信息
        tAction curAct = unitDeal.actTest.at(selRow);
        curAct.actStr = item->text();
        unitDeal.actTest.replace(selRow,curAct);

        if(curAct.actFlag == ACT_KEY)
        {
            ui->stackedWidget->setCurrentWidget(ui->pageKey);
            refreshKeyList(curAct.actStr);
        }
        else if(curAct.actFlag == ACT_SCRIPT)
        {
            ui->stackedWidget->setCurrentWidget(ui->pageScript);
            ui->editFilePath->setText(curAct.actStr);
        }
    }
}

/*************************************************************
/函数功能：名字编辑完成
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::on_editActName_editingFinished()
{
    int selRow = getTableActionSelRanges();
    if(selRow>=unitDeal.actTest.length())
        return ;

    //修改列表信息
    tAction curAct = unitDeal.actTest.at(selRow);
    curAct.actName = ui->editActName->text();
    unitDeal.actTest.replace(selRow,curAct);

    ui->tableAction->setItem(selRow,Col_Name,new QTableWidgetItem(curAct.actName));
}

/*************************************************************
/函数功能：刷新按键列表
/函数参数：动作字符串
/函数返回：wu
*************************************************************/
void defTheUnit::refreshKeyList(QString actStr)
{
    //刷新按键属性
    int keyNum = getKeyNumber(actStr)-1;
    if((keyNum != -1)&&(actStr.isEmpty()==false))
    {
        ui->labelDescript->setText(keyList.at(keyNum).des);
        if(keyList.at(keyNum).type == Can1_1)
        {
            ui->groupKeyONOFF->setEnabled(false);
            ui->groupKeyONOFF->setChecked(false);
        }

        else
        {
            ui->groupKeyONOFF->setEnabled(true);
            if(actStr.contains(":on"))
            {
                ui->groupKeyONOFF->setChecked(true);
                ui->comboKeyList->setCurrentText(actStr.remove(":on"));
            }
            else if(actStr.contains(":off"))
            {
                ui->groupKeyONOFF->setChecked(false);
                ui->comboKeyList->setCurrentText(actStr.remove(":off"));
            }
        }
    }
    else
        ui->comboKeyList->setCurrentIndex(-1);
}

/*************************************************************
/函数功能：选择按键
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::on_comboKeyList_activated(const QString &arg1)
{
    int selRow = getTableActionSelRanges();
    if(selRow>=unitDeal.actTest.length())
        return ;

    //修改列表信息
    tAction curAct = unitDeal.actTest.at(selRow);
    curAct.actStr = arg1;


    int keyNum=getKeyNumber(arg1)-1;
    ui->labelDescript->setText(keyList.at(keyNum).des);
    if(keyList.at(keyNum).type == Can1_1)
        ui->groupKeyONOFF->setEnabled(false);
    else
    {
        ui->groupKeyONOFF->setEnabled(true);
        if(ui->groupKeyONOFF->isChecked())
            curAct.actStr += ":on";
        else
            curAct.actStr +=":off";
    }

    unitDeal.actTest.replace(selRow,curAct);

    ui->tableAction->setItem(selRow,Col_Str,new QTableWidgetItem(curAct.actStr));
}

/*************************************************************
/函数功能：选择按键ONOFF
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::on_groupKeyONOFF_clicked(bool checked)
{
    int selRow = getTableActionSelRanges();
    if(selRow>=unitDeal.actTest.length())
        return ;

    //修改列表信息
    tAction curAct = unitDeal.actTest.at(selRow);
    if(checked)
    {
        if(curAct.actStr.endsWith(":on")==false)
        {
            if((curAct.actStr.endsWith(":off")))
                curAct.actStr.replace(":off",":on");
            else
                curAct.actStr += ":on";
        }
    }
    else
    {
        if(curAct.actStr.endsWith(":off")==false)
        {
            if(curAct.actStr.endsWith(":on"))
                curAct.actStr.replace(":on",":off");
            else
                curAct.actStr += ":off";
        }
    }
    unitDeal.actTest.replace(selRow,curAct);

    ui->tableAction->setItem(selRow,Col_Str,new QTableWidgetItem(curAct.actStr));
}

/*************************************************************
/函数功能： 选择文件或文件夹
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::on_toolButtonSelFile_clicked()
{
    QString dirPath;
    if(ui->checkfileMore->isChecked())
        dirPath=QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("选择脚本文件"),QDir::currentPath()));//选择脚本文件夹
    else
        dirPath=QFileDialog::getOpenFileName(this , tr("选择脚本文件") , "" , tr("Text Files(*.bat)"));//选择脚本文件
    if(!dirPath.isEmpty())
        ui->editFilePath->setText(dirPath);
}

/*************************************************************
/函数功能：脚本是否批量处理
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::on_checkfileMore_clicked(bool checked)
{
    if(checked)
        ui->labelfileShow->setText("文件夹：");
    else
        ui->labelfileShow->setText("文件：");
}

/*************************************************************
/函数功能：脚本路径编辑
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::on_editFilePath_textChanged(const QString &arg1)
{
    int selRow = getTableActionSelRanges();
    if(selRow>=unitDeal.actTest.length())
        return ;

    //修改列表信息
    tAction curAct = unitDeal.actTest.at(selRow);
    curAct.actStr = arg1;
    unitDeal.actTest.replace(selRow,curAct);

    ui->tableAction->setItem(selRow,Col_Str,new QTableWidgetItem(curAct.actStr));
}

/*************************************************************
/函数功能：刷新时间参数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::refreshTimeDeal(tAction act)
{
    int i =0;
    //查询是否存在时间变动的处理
    if(act.changedDeal.isEmpty()==false)
    {
        for(i=0;i<act.changedDeal.length();i++)
        {
            changedParam cngParam = act.changedDeal.at(i);
            if(cngParam.changed==WaitTime)
            {
                ui->editWaitMin->setText(toStr(cngParam.min));
                ui->editWaitMax->setText(toStr(cngParam.max));
                ui->editWaitStep->setText(toStr(cngParam.step));
                break;
            }
        }
    }

    //无-初始化数据为0
    if(i==act.changedDeal.length())
    {
        ui->editWaitMin->setText(toStr(0));
        ui->editWaitMax->setText(toStr(0));
        ui->editWaitStep->setText(toStr(0));
    }

    //刷新时间参数
    ui->editWaitMin->setText(toStr(act.timeDeal.wait));
    ui->editChkTime->setText(toStr(act.timeDeal.check));
    ui->editOverTime->setText(toStr(act.timeDeal.end));
}

/*************************************************************
/函数功能：编辑时间处理槽函数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::editTimeDealSlot()
{
    int selRow = getTableActionSelRanges();
    if(selRow>=unitDeal.actTest.length())
        return ;
    QObject *sender = QObject::sender();
    //修改列表信息
    tAction curAct = unitDeal.actTest.at(selRow);

    if(sender == ui->editChkTime)
        curAct.timeDeal.check = ui->editChkTime->text().toUInt();
    else if(sender == ui->editOverTime)
        curAct.timeDeal.end = ui->editOverTime->text().toUInt();
    else
    {
        if(sender == ui->editWaitStep)
        {
            changedParam cngParam;

            cngParam.changed = WaitTime;

            cngParam.min = ui->editWaitMin->text().toUInt();
            cngParam.max = ui->editWaitMax->text().toUInt();
            cngParam.step = ui->editWaitStep->text().toUInt();

            if((cngParam.step!=0)&&(cngParam.min != cngParam.max))
            {
                int i;

                if(cngParam.max > cngParam.min)
                    cngParam.dir = true;
                else
                    cngParam.dir = false;

                for(i=0;i<curAct.changedDeal.length();i++)
                {
                    if(curAct.changedDeal.at(i).changed == WaitTime)
                    {
                        curAct.changedDeal.replace(i,cngParam);//替换现有参数
                        break;
                    }
                }
                //添加变动处理
                if(i==curAct.changedDeal.length())
                    curAct.changedDeal.append(cngParam);
            }
            else
            {
                //删除变动处理
                for(int i=0;i<curAct.changedDeal.length();i++)
                {
                    if(curAct.changedDeal.at(i).changed == WaitTime)
                    {
                        curAct.changedDeal.removeAt(i);
                        break;
                    }
                }
            }
        }
        else if(sender == ui->editWaitMin)
        {
            curAct.timeDeal.wait = ui->editWaitMin->text().toUInt();
        }
    }

    unitDeal.actTest.replace(selRow,curAct);
}

/*************************************************************
/函数功能：刷新检测处理参数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::refreshCheckDeal(QList<checkParam> chkDeal)
{
    ui->checkSetCurrent->setChecked(false);
    ui->comboCJudge->setCurrentIndex(0);
    ui->editCurMin->setText(toStr(0));
    ui->editCurMax->setText(toStr(0));
    ui->checkSetVolt->setChecked(false);
    ui->comboVJudge->setCurrentIndex(0);
    ui->editCurMin->setText(toStr(0));
    ui->editCurMax->setText(toStr(0));
    ui->checkSetSound->setChecked(false);
    ui->comboSoundChanged->setCurrentIndex(0);
    ui->checkSetLogStr->setChecked(false);
    ui->editScriptString->setText("OK (1 test)");
    ui->checkSetInterface->setChecked(false);
    ui->checkSetPic->setChecked(false);
    ui->radioPicSelf->setChecked(true);
    ui->radioFaceMemory->setChecked(true);


    if(chkDeal.isEmpty()==false)
    {
        for(int i=0;i<chkDeal.length();i++)
        {
            checkParam chkParam = chkDeal.at(i);

            switch(chkParam.check)
            {
            case CHKCurrent:{
                ui->checkSetCurrent->setChecked(true);
                ui->comboCJudge->setCurrentIndex(chkParam.range);
                ui->editCurMin->setText(toStr(chkParam.min));
                ui->editCurMax->setText(toStr(chkParam.max));
                break;
            }
            case CHKVlot:{
                ui->checkSetVolt->setChecked(true);
                ui->comboVJudge->setCurrentIndex(chkParam.range);
                ui->editCurMin->setText(toStr(chkParam.min));
                ui->editCurMax->setText(toStr(chkParam.max));
                break;
            }
            case CHKSound:{
                ui->checkSetSound->setChecked(true);
                ui->comboSoundChanged->setCurrentIndex(chkParam.sound);
                break;
            }
            case CHKScript:{
                ui->checkSetLogStr->setChecked(true);
                ui->editScriptString->setText(chkParam.logContains);
                break;
            }
            case CHKInterface:{
                ui->checkSetInterface->setChecked(true);
                if(chkParam.infoCompare == NoCompare)
                    ui->radioFaceExist->setChecked(true);
                else if(chkParam.infoCompare == MemoryCompare)
                    ui->radioFaceMemory->setChecked(true);
                else if(chkParam.infoCompare == SelfCompare)
                    ui->radioFaceSelf->setChecked(true);
                break;
            }
            case CHKADBPIC:{
                ui->checkSetPic->setChecked(true);
                if(chkParam.infoCompare == NoCompare)
                    ui->radioPicExist->setChecked(true);
                else if(chkParam.infoCompare == MemoryCompare)
                    ui->radioPicMemory->setChecked(true);
                else if(chkParam.infoCompare == SelfCompare)
                    ui->radioPicSelf->setChecked(true);
                else if(chkParam.infoCompare == LocalCompare)
                    ui->radioPicLocal->setChecked(true);
                break;
            }
            case CHKRES:{
                break;
            }
            }
        }
    }
}

/*************************************************************
/函数功能：编辑检测处理槽函数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::editCheckDealSlot(bool checked)
{
    int selRow = getTableActionSelRanges();
    if(selRow>=unitDeal.actTest.length())
        return ;
    QObject *sender = QObject::sender();
    tAction curAct = unitDeal.actTest.at(selRow);
    //启用
    if(checked)
    {
        checkParam chkDeal ;
        initNullChkParam(&chkDeal);
        if(sender == ui->checkSetCurrent)
        {
            chkDeal.check = CHKCurrent;
            chkDeal.range = (range_type_e)ui->comboCJudge->currentIndex();
            chkDeal.min = ui->editCurMin->text().toUInt();
            chkDeal.max = ui->editCurMax->text().toUInt();
            curAct.checkDeal.append(chkDeal);
        }
        else if(sender == ui->checkSetVolt)
        {
            chkDeal.check = CHKVlot;
            chkDeal.range = (range_type_e)ui->comboVJudge->currentIndex();
            chkDeal.min = ui->editVolMin->text().toUInt();
            chkDeal.max = ui->editVolMax->text().toUInt();
            curAct.checkDeal.append(chkDeal);
        }
        else if(sender == ui->checkSetSound)
        {
            chkDeal.check = CHKSound;
            chkDeal.sound = (sound_type_e)ui->comboSoundChanged->currentIndex();
            curAct.checkDeal.append(chkDeal);
        }
        else if(sender == ui->checkSetLogStr)
        {
            chkDeal.check = CHKScript;
            chkDeal.logContains = ui->editScriptString->text();
            curAct.checkDeal.append(chkDeal);
        }
        else if(sender == ui->checkSetInterface)
        {
            chkDeal.check = CHKInterface;

            if(ui->radioFaceExist->isChecked())
                chkDeal.infoCompare = NoCompare;
            else if(ui->radioFaceMemory->isChecked())
                chkDeal.infoCompare = MemoryCompare;
            else if(ui->radioFaceSelf->isChecked())
                chkDeal.infoCompare = SelfCompare;

            curAct.checkDeal.append(chkDeal);
        }
        else if(sender == ui->checkSetPic)
        {
            chkDeal.check = CHKADBPIC;

            if(ui->radioPicExist->isChecked())
                chkDeal.infoCompare = NoCompare;
            else if(ui->radioPicLocal->isChecked())
                chkDeal.infoCompare = LocalCompare;
            else if(ui->radioPicMemory->isChecked())
                chkDeal.infoCompare = MemoryCompare;
            else if(ui->radioPicSelf->isChecked())
                chkDeal.infoCompare = SelfCompare;

            curAct.checkDeal.append(chkDeal);
        }
    }
    //删除
    else
    {
        for(int i=0;i<curAct.checkDeal.length();i++)
        {
            if(((sender == ui->checkSetCurrent)&&(curAct.checkDeal.at(i).check == CHKCurrent))
                    ||((sender == ui->checkSetVolt)&&(curAct.checkDeal.at(i).check == CHKVlot))
                    ||((sender == ui->checkSetSound)&&(curAct.checkDeal.at(i).check == CHKSound))
                    ||((sender == ui->checkSetLogStr)&&(curAct.checkDeal.at(i).check == CHKScript))
                    ||((sender == ui->checkSetInterface)&&(curAct.checkDeal.at(i).check == CHKInterface))
                    ||((sender == ui->checkSetPic)&&(curAct.checkDeal.at(i).check == CHKADBPIC)))
            {
                curAct.checkDeal.removeAt(i);
                break;
            }
        }
    }
    unitDeal.actTest.replace(selRow,curAct);
}


void defTheUnit::on_actLook_triggered()
{
    for(int j=0;j<unitDeal.actTest.length();j++)
    {
        tAction act = unitDeal.actTest.at(j);
        act.infoFlag = 0;
        for(int i=0;i<act.checkDeal.length();i++)
        {
            checkParam chkDat=act.checkDeal.at(i);
            switch(chkDat.check)
            {
            case CHKCurrent:
            act.infoFlag |= COLCURRENT;
            break;
            case CHKSound:
            act.infoFlag |= COLSOUND;
            break;
            //记忆检测：界面检测
            case CHKInterface:
            {
                act.infoFlag |= COLFACE;
                act.infoFlag |= COLFACESITE;//动作执行后采集
                goto ADDINFODAT;
                break;
            }
            //图片检测：
            case CHKADBPIC:
            {
                act.infoFlag |= COLPICTURE;
                act.infoFlag |= COLPICTURESITE;//动作执行后采集

                if(chkDat.infoCompare != MemoryCompare)
                    break;//非记忆比较，均无需对应动作获取信息

                //非比较首次图片，即与测试过程中其他图片比较
                ADDINFODAT:
                bool curStatus ;

                if(act.actStr.contains(":on"))
                    curStatus=true;
                else
                    curStatus=false;

                //同时查找该动作之前测试单元中前一个状态下 添加上动作执行前采集当前界面信息
                for(int front=0;front<unitDeal.actTest.length();front++)
                {
                    QString actStr=act.actStr;
                    QString unitfindStr = unitDeal.actTest.at(front).actStr;

                    if(((curStatus)&&((unitfindStr.contains(":off"))&&(unitfindStr.contains(actStr.remove(":on")))))
                     ||((!curStatus)&&((unitfindStr.contains(":on"))&&(unitfindStr.contains(actStr.remove(":off"))))))
                    {
                        tAction findAction = unitDeal.actTest.at(front);

                        findAction.infoFlag |= COLFACE;
                        if(chkDat.check == CHKInterface)
                            findAction.infoFlag &= ~(1<<5);//COLFACESITE
                        else if(chkDat.check == CHKADBPIC)
                            findAction.infoFlag &= ~(1<<7);//COLPICTURESITE

                        unitDeal.actTest.replace(front,findAction);
                    }
                }
                break;
            }
            default:break;
            }
            unitDeal.actTest.replace(j,act);
        }
    }
    CfgLookUnit lookUnit(&unitDeal);

    lookUnit.exec();//any：是否可修改
}

