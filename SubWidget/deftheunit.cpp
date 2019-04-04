#include "deftheunit.h"
#include "ui_deftheunit.h"

defTheUnit::defTheUnit(tUnit *unit,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::defTheUnit)
{
    ui->setupUi(this);

    //初始化窗口界面
    ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //tab标签栏优化
    ui->tabProperties->tabBar()->setStyle(new CustomTabStyle);
    ui->tabProperties->setCurrentWidget(ui->tabAct);
    ui->tabChkParam->setCurrentWidget(ui->tabCurrent);

    //获取文件中存储的测试单元
    Model_XMLFile xmlRead;
    int len = unitList.length();
    xmlRead.readSequenceXML(configPath("unitDefined.xml"),unitList);

    for(int i=len;i<unitList.length();i++)
    {
        if(userLogin.Permissions != OnlyUser)
            ui->listUnit->addItem(unitList.at(i).name);
    }

    ui->comboBoxActColInfo->appendItem("Interface:Front", false);
    ui->comboBoxActColInfo->appendItem("Interface:Back", false);
    ui->comboBoxActColInfo->appendItem("Picture:Front", false);
    ui->comboBoxActColInfo->appendItem("Picture:Back", false);

    //初始化按键列表
    xmlRead.readKeyInfoXML(WorkItem,&keyList);

    for(int i=0;i<keyList.length();i++)
    {
        if(keyList.at(i).isUse)
            ui->comboKeyList->addItem("KEY"+QString::number(i+1)+":"+keyList.at(i).name);
    }
    ui->comboKeyList->setCurrentIndex(-1);

    //初始化当前测试单元
    if((unit->name.isEmpty())&&(unit->actTest.isEmpty()))
    {
        this->setWindowTitle("Add-Unit");
        //创立之初为空
        on_actnew_triggered();

    }
    else
    {
        this->setWindowTitle("Edit-Unit");
        unitDeal = *unit;

        ui->editUnitName->setText(unitDeal.name);
        ui->spinUnitCycle->setValue(unitDeal.cycleCount);
        ui->editUnitDes->setText(unitDeal.unitDes);

        ui->editUnitName->setEnabled(false);

        //clear
        for(uint16_t i=ui->tableAction->rowCount();i>0;i--)
        {
            ui->tableAction->removeRow(i-1);
        }

        //append
        for(int i=0;i<unitDeal.actTest.length();i++)
        {
            int row=ui->tableAction->rowCount();
            ui->tableAction->setRowCount(row+1);

            ui->tableAction->selectRow(row);
            ui->tableAction->setItem(row,Col_Name,new QTableWidgetItem(unitDeal.actTest.at(i).actName));
            ui->tableAction->setItem(row,Col_Str,new QTableWidgetItem(unitDeal.actTest.at(i).actStr));

            refreshPropertiesParam(i,unitDeal.actTest.at(i));
        }
    }

    //创建窗口组件信号槽函数
    connect(ui->editWaitMin,SIGNAL(textChanged(QString)),this,SLOT(editTimeDealSlot(QString)));
    connect(ui->editWaitMax,SIGNAL(textChanged(QString)),this,SLOT(editTimeDealSlot(QString)));
    connect(ui->editWaitStep,SIGNAL(textChanged(QString)),this,SLOT(editTimeDealSlot(QString)));
    connect(ui->editChkTime,SIGNAL(textChanged(QString)),this,SLOT(editTimeDealSlot(QString)));
    connect(ui->editOverTime,SIGNAL(textChanged(QString)),this,SLOT(editTimeDealSlot(QString)));

    connect(ui->checkSetCurrent,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));
    connect(ui->checkSetVolt,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));
    connect(ui->checkSetSound,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));
    connect(ui->checkSetLogStr,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));
    connect(ui->checkSetInterface,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));
    connect(ui->checkSetPic,SIGNAL(clicked(bool)),this,SLOT(editCheckDealSlot(bool)));

    connect(ui->radioBtnNODeal,SIGNAL(clicked()),this,SLOT(editErrorDealSlot()));
    connect(ui->radioBtnOverTimeDeal,SIGNAL(clicked()),this,SLOT(editErrorDealSlot()));

    if(userLogin.Permissions == OnlyUser)
    {
        ui->actSave->setEnabled(false);
        ui->actnew->setEnabled(false);
        ui->listUnit->setEnabled(false);
    }
}

defTheUnit::~defTheUnit()
{
    delete ui;
}

/*************************************************************
/函数功能：初始化动作参数
/函数参数：动作
/函数返回：wu
*************************************************************/
void defTheUnit::inittActionParam(tAction *tact)
{
    tact->actName = "";
    tact->actStr =  "";
    tact->timeDeal.wait = 0;
    tact->timeDeal.check = 0;
    tact->timeDeal.end = 0;

    //tact->infoFlag=0;
    tact->errorDeal=0;
    tact->colInfoList.clear();
    tact->checkDeal.clear();
    tact->changedDeal.clear();
}

/*************************************************************
/函数功能：测试单元名修改
/函数参数：
/函数返回：wu
*************************************************************/
void defTheUnit::on_editUnitName_textChanged(const QString &arg1)
{
    unitDeal.name = arg1;
}

/*************************************************************
/函数功能：测试次数修改
/函数参数：
/函数返回：wu
*************************************************************/
void defTheUnit::on_spinUnitCycle_valueChanged(int arg1)
{
    unitDeal.cycleCount = arg1;
}

/*************************************************************
/函数功能：测试单元描述
/函数参数：
/函数返回：wu
*************************************************************/
void defTheUnit::on_editUnitDes_textChanged()
{
    unitDeal.unitDes = ui->editUnitDes->toPlainText();
}

/*************************************************************
/函数功能：tableAction右键创建菜单栏
/函数参数：位置
/函数返回：wu
*************************************************************/
void defTheUnit::on_tableAction_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    QMenu *popMenu = new QMenu( this );
    QAction *deleteAct = new QAction(tr("delete"), this);
    QAction *upAct = new QAction(tr("上移"), this);
    QAction *downAct = new QAction(tr("下移"), this);
    QAction *ScriptAction = new QAction(tr("Add Script"), this);

    QMenu *keyMenu = new QMenu("Add Key");
    QAction *ACCONAction = new QAction(tr("ACCON"), this);
    QAction *ACCOFFAction = new QAction(tr("ACCOFF"), this);
    QAction *BATONAction = new QAction(tr("BATON"), this);
    QAction *BATOFFAction = new QAction(tr("BATOFF"), this);
    QAction *CCDONAction = new QAction(tr("CCDON"), this);
    QAction *CCDOFFAction = new QAction(tr("CCDOFF"), this);
    QAction *OtherAction = new QAction(tr("Other"), this);
    keyMenu->addAction(ACCONAction);
    keyMenu->addAction(ACCOFFAction);
    keyMenu->addAction(BATONAction);
    keyMenu->addAction(BATOFFAction);
    keyMenu->addAction(CCDONAction);
    keyMenu->addAction(CCDOFFAction);
    keyMenu->addAction(OtherAction);

    if(userLogin.Permissions == Administrator)
    {
        popMenu->addMenu(keyMenu);
        popMenu->addAction(ScriptAction);
        popMenu->addSeparator();
        popMenu->addAction(upAct);
        popMenu->addAction(downAct);
        popMenu->addAction(deleteAct);
    }

    connect( ACCONAction,        SIGNAL(triggered() ), this, SLOT( ACCONActionSlot()) );
    connect( ACCOFFAction,        SIGNAL(triggered() ), this, SLOT( ACCOFFActionSlot()) );
    connect( BATONAction,        SIGNAL(triggered() ), this, SLOT( BATONActionSlot()) );
    connect( BATOFFAction,        SIGNAL(triggered() ), this, SLOT( BATOFFActionSlot()) );
    connect( CCDONAction,        SIGNAL(triggered() ), this, SLOT( CCDONActionSlot()) );
    connect( CCDOFFAction,        SIGNAL(triggered() ), this, SLOT( CCDOFFActionSlot()) );
    connect( OtherAction,        SIGNAL(triggered() ), this, SLOT( keyActionSlot()) );

    connect( ScriptAction,        SIGNAL(triggered() ), this, SLOT( scriptActionSlot()) );

    connect( deleteAct,        SIGNAL(triggered() ), this, SLOT( deleteActionSlot()) );
    connect( upAct,        SIGNAL(triggered() ), this, SLOT( upActionSlot()) );
    connect( downAct,        SIGNAL(triggered() ), this, SLOT( downActionSlot()) );

    popMenu->exec( QCursor::pos() );

    delete popMenu;

}

/*************************************************************
/函数功能：上移
/函数参数：
/函数返回：无
*************************************************************/
void defTheUnit::upActionSlot()
{
    QTableWidgetItem * item = ui->tableAction->currentItem();
    if( item == NULL )
        return;

    int curIndex = ui->tableAction->row(item);

    moveRow(curIndex, curIndex-1 );
}

/*************************************************************
/函数功能：下移
/函数参数：
/函数返回：无
*************************************************************/
void defTheUnit::downActionSlot()
{
    QTableWidgetItem * item = ui->tableAction->currentItem();
    if( item == NULL )
        return;

    int curIndex = ui->tableAction->row(item);
    moveRow(curIndex, curIndex+1 );
}

/*************************************************************
/函数功能：移动行：表格显示及序列排序
/函数参数：
/函数返回：无
*************************************************************/
void defTheUnit::moveRow(int nFrom, int nTo )
{
    if( ui->tableAction == NULL )
        return;
    ui->tableAction->setFocus();

    if( nFrom == nTo )
        return;
    if( nFrom < 0 || nTo < 0 )
        return;
    int nRowCount = ui->tableAction->rowCount();
    if( nFrom >= nRowCount  || nTo > nRowCount )
        return;
    if( nFrom >= unitDeal.actTest.length()  || nTo > unitDeal.actTest.length() )
        return;

    //交换序列
    tAction testAct=unitDeal.actTest.at(nFrom);
    tAction swopAct=unitDeal.actTest.at(nTo);

    for(int i=0;i<testAct.colInfoList.length();i++)
    {
        QString colInfoStr = testAct.colInfoList.at(i);
        colInfoStr.replace("ACT"+toStr(nFrom+1),"ACT"+toStr(nTo+1));
        testAct.colInfoList.replace(i,colInfoStr);
    }

    for(int i=0;i<swopAct.colInfoList.length();i++)
    {
        QString colInfoStr = swopAct.colInfoList.at(i);
        colInfoStr.replace("ACT"+toStr(nTo+1),"ACT"+toStr(nFrom+1));
        swopAct.colInfoList.replace(i,colInfoStr);
    }

    unitDeal.actTest.replace(nFrom,swopAct);
    unitDeal.actTest.replace(nTo,testAct);

    //交换表格显示表格显示
    if( nTo < nFrom )
        nFrom++;
    else
        nTo++;

    ui->tableAction->insertRow( nTo );//即在当前号增加一个空的行，原有数据下移

    //取消链接，避免设置项目时，跳转到处理项目改变槽函数，影响测试动作改变
   // disconnect(ui->tableAction,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(on_tableAction_itemChanged(QTableWidgetItem*)));
    for( int i=0; i<ui->tableAction->columnCount(); i++ )
    {
        ui->tableAction->setItem( nTo, i, ui->tableAction->takeItem( nFrom , i ) );
    }
   // connect(ui->tableAction,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(on_tableAction_itemChanged(QTableWidgetItem*)));

    if( nFrom < nTo  )
        nTo--;

    ui->tableAction->removeRow( nFrom );
    ui->tableAction->selectRow( nTo );
}

/*************************************************************
/函数功能：删除行
/函数参数：
/函数返回：无
*************************************************************/
void defTheUnit::deleteActionSlot()
{
    QTableWidgetItem * item = ui->tableAction->currentItem();
    if( item == NULL )
        return;

    int curIndex = ui->tableAction->row(item);
    ui->tableAction->removeRow(curIndex);          //delete item;因已移除行，无需再删除项目

    if((curIndex>=0)&&(curIndex<unitDeal.actTest.length()))
    {
        unitDeal.actTest.removeAt(curIndex);
        /*删除当前动作后，后面动作的采集标号修改*/
        for(uint8_t i=curIndex;i<unitDeal.actTest.length();i++)
        {
            tAction tempAct = unitDeal.actTest.at(i);
            if(tempAct.colInfoList.isEmpty()==false)
            {
                for(int j=0;j<tempAct.colInfoList.length();j++)
                {
                    QString colInfoStr = tempAct.colInfoList.at(j);
                    colInfoStr.replace("ACT"+toStr(i+2),"ACT"+toStr(i+1));
                    //cout << colInfoStr<<i;
                    tempAct.colInfoList.replace(j,colInfoStr);
                }
                unitDeal.actTest.replace(i,tempAct);
            }
        }
    }
}

/*************************************************************
/函数功能：ACCON槽函数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::ACCONActionSlot()
{
    tAction kAction;
    inittActionParam(&kAction);
    kAction.actName = "ACCON";
    kAction.actFlag = ACT_KEY;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardACC))
        {
            kAction.actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name+":on";
            break;
        }
    }

    kAction.timeDeal.wait = 5000;
    kAction.timeDeal.check = 0;
    kAction.timeDeal.end = 60000;

    checkParam checkCurrent;
    initNullChkParam(&checkCurrent);
    checkCurrent.check = CHKCurrent;
    checkCurrent.range = GE;
    checkCurrent.min = WorkCurrent;
    kAction.checkDeal.append(checkCurrent);

   /* checkParam checkMemoey;
    initNullChkParam(&checkMemoey);
    checkMemoey.check = CHKInterface;
    checkMemoey.infoCompare = MemoryCompare;
    kAction.checkDeal.append(checkMemoey);*/

    int row=ui->tableAction->rowCount();
    //kAction.colInfoList.append("ACT"+toStr(row+1)+":Interface:Back");
    kAction.colInfoList.append("ACT"+toStr(row+1)+":Current");

    /*checkParam checkSound;
    initNullChkParam(&checkSound);
    checkSound.check = CHKSound;
    checkSound.sound = noNSoundCount;
    kAction.checkDeal.append(checkSound);*/

    appendTableAction(kAction);
}

/*************************************************************
/函数功能：按键动作添加槽函数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::ACCOFFActionSlot()
{
    tAction kAction;
    inittActionParam(&kAction);
    kAction.actName = "ACCOFF";
    kAction.actFlag = ACT_KEY;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardACC))
        {
            kAction.actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name+":off";
            break;
        }
    }

    kAction.timeDeal.wait = 10000;
    kAction.timeDeal.check = 15000;
    kAction.timeDeal.end = 60000;

    int row=ui->tableAction->rowCount();
    kAction.colInfoList.append("ACT"+toStr(row+1)+":Interface:Front");
    kAction.colInfoList.append("ACT"+toStr(row+1)+":Current");

    changedParam changeTime;
    changeTime.changed = WaitTime;
    changeTime.dir = true;
    changeTime.min  = kAction.timeDeal.wait;
    changeTime.max  = 30000;
    changeTime.step = 100;
    kAction.changedDeal.append(changeTime);

    checkParam checkCurrent;
    initNullChkParam(&checkCurrent);
    checkCurrent.check = CHKCurrent;
    checkCurrent.range = LE;
    checkCurrent.min = 20;
    kAction.checkDeal.append(checkCurrent);

    appendTableAction(kAction);
}

/*************************************************************
/函数功能：按键动作添加槽函数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::BATONActionSlot()
{
    tAction kAction;
    inittActionParam(&kAction);
    kAction.actName = "BATON";
    kAction.actFlag = ACT_KEY;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardBAT))
        {
            kAction.actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name+":on";
            break;
        }
    }

    kAction.timeDeal.wait = 10000;
    kAction.timeDeal.check = 0;
    kAction.timeDeal.end = 60000;


    checkParam checkCurrent;
    initNullChkParam(&checkCurrent);
    checkCurrent.check = CHKCurrent;
    checkCurrent.range = GE;
    checkCurrent.min = WorkCurrent;
    kAction.checkDeal.append(checkCurrent);

   /* checkParam checkMemoey;
    initNullChkParam(&checkMemoey);
    checkMemoey.check = CHKInterface;
    checkMemoey.infoCompare = MemoryCompare;
    kAction.checkDeal.append(checkMemoey);*/

    int row=ui->tableAction->rowCount();
    //kAction.colInfoList.append("ACT"+toStr(row+1)+":Interface:Back");
    kAction.colInfoList.append("ACT"+toStr(row+1)+":Current");

    /*checkParam checkSound;
    initNullChkParam(&checkSound);
    checkSound.check = CHKSound;
    checkSound.sound = noNSoundCount;
    kAction.checkDeal.append(checkSound);*/

    appendTableAction(kAction);
}

/*************************************************************
/函数功能：按键动作添加槽函数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::BATOFFActionSlot()
{
    tAction kAction;
    inittActionParam(&kAction);
    kAction.actName = "BATOFF";
    kAction.actFlag = ACT_KEY;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardBAT))
        {
            kAction.actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name+":off";
            break;
        }
    }

    kAction.timeDeal.wait = 5000;
    kAction.timeDeal.check = 0;
    kAction.timeDeal.end = 60000;

    int row=ui->tableAction->rowCount();
    kAction.colInfoList.append("ACT"+toStr(row+1)+":Interface:Front");

    changedParam changeTime;
    changeTime.changed = WaitTime;
    changeTime.dir = true;
    changeTime.min  = kAction.timeDeal.wait;
    changeTime.max  = 30000;
    changeTime.step = 100;
    kAction.changedDeal.append(changeTime);

    appendTableAction(kAction);
}

/*************************************************************
/函数功能：按键动作添加槽函数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::CCDONActionSlot()
{
    tAction kAction;
    inittActionParam(&kAction);
    kAction.actName = "CCDON";
    kAction.actFlag = ACT_KEY;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardCCD))
        {
            kAction.actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name+":on";
            break;
        }
    }

    kAction.timeDeal.wait = 5000;
    kAction.timeDeal.check = 0;
    kAction.timeDeal.end = 60000;

    int row=ui->tableAction->rowCount();
    kAction.colInfoList.append("ACT"+toStr(row+1)+":Interface:Front");
    kAction.colInfoList.append("ACT"+toStr(row+1)+":Picture:Back");

    changedParam changeTime;
    changeTime.changed = WaitTime;
    changeTime.dir = true;
    changeTime.min  = kAction.timeDeal.wait;
    changeTime.max  = 30000;
    changeTime.step = 100;
    kAction.changedDeal.append(changeTime);

    checkParam checkPicture;
    initNullChkParam(&checkPicture);
    checkPicture.check = CHKADBPIC;
    checkPicture.infoCompare = SelfCompare;
    kAction.checkDeal.append(checkPicture);

    appendTableAction(kAction);
}

/*************************************************************
/函数功能：按键动作添加槽函数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::CCDOFFActionSlot()
{
    tAction kAction;
    inittActionParam(&kAction);
    kAction.actName = "CCDOFF";
    kAction.actFlag = ACT_KEY;

    for(int i=0;i<keyList.length();i++)
    {
        if((keyList.at(i).isUse)&&(keyList.at(i).type == HardCCD))
        {
            kAction.actStr = "KEY"+QString::number(i+1)+":"+keyList.at(i).name+":off";
            break;
        }
    }

    kAction.timeDeal.wait = 5000;
    kAction.timeDeal.check = 0;
    kAction.timeDeal.end = 60000;

    /*
    checkParam checkMemoey;
    initNullChkParam(&checkMemoey);
    checkMemoey.check = CHKInterface;
    checkMemoey.infoCompare = MemoryCompare;
    kAction.checkDeal.append(checkMemoey);

    int row=ui->tableAction->rowCount();
    kAction.colInfoList.append("ACT"+toStr(row+1)+":Interface:Back");
*/
    /*checkParam checkSound;
    initNullChkParam(&checkSound);
    checkSound.check = CHKSound;
    checkSound.sound = noNSoundCount;
    kAction.checkDeal.append(checkSound);*/

    appendTableAction(kAction);
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

    sAction.timeDeal.end = 60000;

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
    refreshPropertiesParam(row,act);
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
    refreshPropertiesParam(index.row(),selAction);
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
void defTheUnit::refreshPropertiesParam(int index,tAction act)
{
    ui->editActName->setText(act.actName);
    refreshColInfo(act.colInfoList);

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

        if((act.actStr.isEmpty())||(act.actStr.endsWith(".bat"))||(act.actStr.endsWith(".BAT")))
            ui->checkfileMore->setChecked(false);
        else
            ui->checkfileMore->setChecked(true);
    }

    //刷新时间属性
    refreshTimeDeal(act);
    //刷新检测属性
    ui->comboChkParamFace->clear();
    ui->comboChkParamPic->clear();
    for(int i=0;i<index;i++)
    {
        tAction ActDeal = unitDeal.actTest.at(i);

        for(int j=0;j<ActDeal.colInfoList.length();j++)
        {
            if(ActDeal.colInfoList.at(j).contains("Interface"))
                ui->comboChkParamFace->addItem(ActDeal.colInfoList.at(j));
            else if(ActDeal.colInfoList.at(j).contains("Picture"))
                ui->comboChkParamPic->addItem(ActDeal.colInfoList.at(j));
        }
    }
    refreshCheckDeal(act.checkDeal);

    refreshErrorDeal(act.errorDeal);

}
#if 0  //列表不可编辑
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
            if((curAct.actStr.isEmpty())||(curAct.actStr.endsWith(".bat"))||(curAct.actStr.endsWith(".BAT")))
            {
                ui->checkfileMore->setChecked(false);
            }
            else
                ui->checkfileMore->setChecked(true);
        }
    }
}
#endif

/*************************************************************
/函数功能：名字编辑完成
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::on_editActName_textChanged(const QString &arg1)
{
    int selRow = getTableActionSelRanges();
    if(selRow>=unitDeal.actTest.length())
        return ;

    //修改列表信息
    tAction curAct = unitDeal.actTest.at(selRow);
    curAct.actName = arg1;
    unitDeal.actTest.replace(selRow,curAct);

    ui->tableAction->setItem(selRow,Col_Name,new QTableWidgetItem(curAct.actName));
}

/*************************************************************
/函数功能：刷新采集信息列表
/函数参数：采集列表
/函数返回：wu
*************************************************************/
void defTheUnit::refreshColInfo(QStringList infoDeal)
{
    for(int i=0;i<ui->comboBoxActColInfo->count();i++)
    {
        ui->comboBoxActColInfo->clickedItem(i,false);
    }

    for(int i=0;i<infoDeal.length();i++)
    {
        QString infoStr = infoDeal.at(i);
        QStringList infolist = infoStr.split(":");
        if(infolist.length() == 3)
        {
            if(infolist.at(1) == "Interface")
            {
                if(infolist.at(2) == "Front")
                    ui->comboBoxActColInfo->clickedItem(ACTFront_Interface,true);
                else if(infolist.at(2) == "Back")
                    ui->comboBoxActColInfo->clickedItem(ACTBack_Interface,true);
            }
            else if(infolist.at(1) == "Picture")
            {
                if(infolist.at(2) == "Front")
                    ui->comboBoxActColInfo->clickedItem(ACTFront_Picture,true);
                else if(infolist.at(2) == "Back")
                    ui->comboBoxActColInfo->clickedItem(ACTBack_Picture,true);
            }
        }
    }
}

/*************************************************************
/函数功能：状态改变
/函数参数：项目指针，及状态
/函数返回：wu anydeal
*************************************************************/
void defTheUnit::on_comboBoxActColInfo_checkedStateChange(int index, bool checked)
{
    int selRow = getTableActionSelRanges();
    if(selRow>=unitDeal.actTest.length())
        return ;

    //修改列表信息
    tAction curAct = unitDeal.actTest.at(selRow);
    QString infoStr;

    switch(index)
    {
    case ACTFront_Interface:infoStr ="ACT"+toStr(selRow+1)+":Interface:Front";break;
    case ACTBack_Interface:infoStr ="ACT"+toStr(selRow+1)+":Interface:Back";break;
    case ACTFront_Picture:infoStr ="ACT"+toStr(selRow+1)+":Picture:Front";break;
    case ACTBack_Picture:infoStr ="ACT"+toStr(selRow+1)+":Picture:Back";break;
    }

    if(checked)
    {
         curAct.colInfoList.append(infoStr);//加
    }
    else
    {
        curAct.colInfoList.removeOne(infoStr);//减
    }
    unitDeal.actTest.replace(selRow,curAct);
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
        if((keyList.at(keyNum).type == Can1_1)||(keyList.at(keyNum).type == Can2_1))
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
    if((keyList.at(keyNum).type == Can1_1)||(keyList.at(keyNum).type == Can2_1))
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
        ui->labelfileShow->setText(tr("文件夹："));
    else
        ui->labelfileShow->setText(tr("文件："));
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
void defTheUnit::editTimeDealSlot(QString arg1)
{
    int selRow = getTableActionSelRanges();
    if(selRow>=unitDeal.actTest.length())
        return ;
    QObject *sender = QObject::sender();
    //修改列表信息
    tAction curAct = unitDeal.actTest.at(selRow);

    if(sender == ui->editChkTime)
        curAct.timeDeal.check = arg1.toUInt();
    else if(sender == ui->editOverTime)
        curAct.timeDeal.end = arg1.toUInt();
    else
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
        curAct.timeDeal.wait = cngParam.min;
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

    ui->groupBox_cur->setEnabled(true);
    ui->groupBox_vol->setEnabled(true);
    ui->groupBox_sound->setEnabled(true);
    ui->groupBox_log->setEnabled(true);
    ui->groupBox_face->setEnabled(true);
    ui->groupBox_pic->setEnabled(true);
    ui->comboChkParamFace->setCurrentIndex(0);
    ui->comboChkParamPic->setCurrentIndex(0);


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
                ui->groupBox_cur->setEnabled(false);
                break;
            }
            case CHKVlot:{
                ui->checkSetVolt->setChecked(true);
                ui->comboVJudge->setCurrentIndex(chkParam.range);
                ui->editCurMin->setText(toStr(chkParam.min));
                ui->editCurMax->setText(toStr(chkParam.max));
                ui->groupBox_pic->setEnabled(false);
                break;
            }
            case CHKSound:{
                ui->checkSetSound->setChecked(true);
                ui->comboSoundChanged->setCurrentIndex(chkParam.sound);
                ui->groupBox_sound->setEnabled(false);
                break;
            }
            case CHKScript:{
                ui->checkSetLogStr->setChecked(true);
                ui->editScriptString->setText(chkParam.logContains);
                ui->groupBox_log->setEnabled(false);
                break;
            }
            case CHKInterface:{
                ui->checkSetInterface->setChecked(true);
                if(chkParam.infoCompare == NoCompare)
                    ui->radioFaceExist->setChecked(true);
                else if(chkParam.infoCompare == MemoryCompare)
                {
                    ui->radioFaceMemory->setChecked(true);
                    ui->comboChkParamFace->setCurrentText(chkParam.comTarget);
                }
                else if(chkParam.infoCompare == SelfCompare)
                    ui->radioFaceSelf->setChecked(true);
                ui->groupBox_face->setEnabled(false);
                break;
            }
            case CHKADBPIC:{
                ui->checkSetPic->setChecked(true);
                if(chkParam.infoCompare == NoCompare)
                    ui->radioPicExist->setChecked(true);
                else if(chkParam.infoCompare == MemoryCompare)
                {
                    ui->radioPicMemory->setChecked(true);
                    ui->comboChkParamPic->setCurrentText(chkParam.comTarget);
                }
                else if(chkParam.infoCompare == SelfCompare)
                    ui->radioPicSelf->setChecked(true);
                else if(chkParam.infoCompare == LocalCompare)
                    ui->radioPicLocal->setChecked(true);
                ui->groupBox_pic->setEnabled(false);
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

            curAct.colInfoList.append("ACT"+toStr(selRow+1)+":Current");

            ui->groupBox_cur->setEnabled(false);
        }
        else if(sender == ui->checkSetVolt)
        {
            chkDeal.check = CHKVlot;
            chkDeal.range = (range_type_e)ui->comboVJudge->currentIndex();
            chkDeal.min = ui->editVoltMin->text().toUInt();
            chkDeal.max = ui->editVoltMax->text().toUInt();
            curAct.checkDeal.append(chkDeal);

            ui->groupBox_vol->setEnabled(false);
        }
        else if(sender == ui->checkSetSound)
        {
            chkDeal.check = CHKSound;
            chkDeal.sound = (sound_type_e)ui->comboSoundChanged->currentIndex();
            curAct.checkDeal.append(chkDeal);
            curAct.colInfoList.append("ACT"+toStr(selRow+1)+":Sound");

            ui->groupBox_sound->setEnabled(false);
        }
        else if(sender == ui->checkSetLogStr)
        {
            chkDeal.check = CHKScript;
            chkDeal.logContains = ui->editScriptString->text();
            curAct.checkDeal.append(chkDeal);

            ui->groupBox_log->setEnabled(false);
        }
        else if(sender == ui->checkSetInterface)
        {
            chkDeal.check = CHKInterface;

            if(ui->radioFaceExist->isChecked())
                chkDeal.infoCompare = NoCompare;
            else if(ui->radioFaceMemory->isChecked())
            {
                chkDeal.infoCompare = MemoryCompare;
                chkDeal.comTarget = ui->comboChkParamFace->currentText();
            }
            else if(ui->radioFaceSelf->isChecked())
                chkDeal.infoCompare = SelfCompare;

            curAct.checkDeal.append(chkDeal);

            curAct.colInfoList.append("ACT"+toStr(selRow+1)+":Interface:Back");
            ui->comboBoxActColInfo->clickedItem(ACTBack_Interface,true);

            ui->groupBox_face->setEnabled(false);
        }
        else if(sender == ui->checkSetPic)
        {
            chkDeal.check = CHKADBPIC;

            if(ui->radioPicExist->isChecked())
                chkDeal.infoCompare = NoCompare;
            else if(ui->radioPicLocal->isChecked())
                chkDeal.infoCompare = LocalCompare;
            else if(ui->radioPicMemory->isChecked())
            {
                chkDeal.infoCompare = MemoryCompare;
                chkDeal.comTarget = ui->comboChkParamPic->currentText();
            }
            else if(ui->radioPicSelf->isChecked())
                chkDeal.infoCompare = SelfCompare;

            curAct.checkDeal.append(chkDeal);

            curAct.colInfoList.append("ACT"+toStr(selRow+1)+":Picture:Back");
            ui->comboBoxActColInfo->clickedItem(ACTBack_Picture,true);

            ui->groupBox_pic->setEnabled(false);
        }
    }
    //删除
    else
    {
        if(sender == ui->checkSetCurrent)
        {
            ui->groupBox_cur->setEnabled(true);
            curAct.colInfoList.removeOne("ACT"+toStr(selRow+1)+":Current");
        }
        else if(sender == ui->checkSetVolt)
            ui->groupBox_vol->setEnabled(true);
        else if(sender == ui->checkSetSound)
        {
            ui->groupBox_sound->setEnabled(true);
            curAct.colInfoList.removeOne("ACT"+toStr(selRow+1)+":Sound");
        }
        else if(sender == ui->checkSetLogStr)
            ui->groupBox_log->setEnabled(true);
        else if(sender == ui->checkSetInterface)
        {
            ui->groupBox_face->setEnabled(true);
            curAct.colInfoList.removeOne("ACT"+toStr(selRow+1)+":Interface:Back");
            ui->comboBoxActColInfo->clickedItem(ACTBack_Interface,false);
        }
        else if(sender == ui->checkSetPic)
        {
            ui->groupBox_pic->setEnabled(true);
            curAct.colInfoList.removeOne("ACT"+toStr(selRow+1)+":Picture:Back");
            ui->comboBoxActColInfo->clickedItem(ACTBack_Picture,false);
        }

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

void defTheUnit::refreshErrorDeal(uint8_t errorFlag)
{
    if(errorFlag == NODealERROR)
        ui->radioBtnNODeal->setChecked(true);
    else if(errorFlag == OVERTIMEERR)
        ui->radioBtnOverTimeDeal->setChecked(true);
}

void defTheUnit::editErrorDealSlot()
{
    int selRow = getTableActionSelRanges();
    if(selRow>=unitDeal.actTest.length())
        return ;
    QObject *sender = QObject::sender();
    //修改列表信息
    tAction curAct = unitDeal.actTest.at(selRow);

    if(sender == ui->radioBtnNODeal)
    {
        curAct.errorDeal = NODealERROR;
    }
    else if(sender == ui->radioBtnOverTimeDeal)
    {
        curAct.errorDeal = OVERTIMEERR;
    }
    unitDeal.actTest.replace(selRow,curAct);
}

/*************************************************************
/函数功能：修改INFO信息采集标志
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::changedInfoFlagDeal()
{


#if 0
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
#endif
}

/*************************************************************
/函数功能：预览
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::on_actLook_triggered()
{
    if((unitDeal.actTest.isEmpty())||(unitDeal.name.isEmpty()))
    {
        QMessageBox::warning(NULL, tr("提示"), tr("请输入测试单元名/检测测试动作是否为空！"));
        return ;
    }
    changedInfoFlagDeal();
    CfgLookUnit lookUnit(&unitDeal);

    lookUnit.exec();//不可修改,该窗口即为配置窗口，无需再修改
}

/*************************************************************
/函数功能：保存测试单元
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::on_actSave_triggered()
{
    if((unitDeal.actTest.isEmpty())||(unitDeal.name.isEmpty()))
    {
        QMessageBox::warning(NULL, tr("提示"), tr("请输入测试单元名/检测测试动作是否为空！"));
        return ;
    }

    changedInfoFlagDeal();

    QFile file(configPath("unitDefined.xml")); //关联文件名字
    Model_XMLFile xmlSave;
    QStringList infoList;

    if( false == file.exists() ) //如果存在不创建
    {
        xmlSave.createSequenceXML(configPath("unitDefined.xml"));
    }
    //查询是否已经存在该测试单元
    if(xmlSave.hasUnitInfomation(configPath("unitDefined.xml"),unitDeal.name))
    {
        if(QMessageBox::information(NULL, tr("提示"), tr("文件中该测试单元已存在，是否替换？？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)==QMessageBox::No)
            return;
        else
        {
            xmlSave.removeUnitXML(configPath("unitDefined.xml"),unitDeal.name);
            for(int i=0;i<unitList.length();i++)
            {
                if(unitList.at(i).name==unitDeal.name)
                    unitList.replace(i,unitDeal);
            }
        }
    }
    else
    {
        ui->listUnit->addItem(unitDeal.name);
        unitList.append(unitDeal);
    }

    infoList.clear();
    infoList<<unitDeal.name<<QString::number(unitDeal.cycleCount)<<unitDeal.unitDes;//"this is test"

    for(int j=0;j<unitDeal.actTest.length();j++)
    {
        xmlSave.appendSequenceXML(configPath("unitDefined.xml"),infoList,unitDeal.actTest.at(j));
    }

}

/*************************************************************
/函数功能：应用测试单元
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::on_actApply_triggered()
{
    if((unitDeal.actTest.isEmpty())||(unitDeal.name.isEmpty()))
    {
        QMessageBox::warning(NULL, tr("提示"), tr("请输入测试单元名/检测测试动作是否为空！"));
        return ;
    }

    changedInfoFlagDeal();
    applyTheUnit(unitDeal);
}

/*************************************************************
/函数功能：选择当前测试单元
/函数参数：指针
/函数返回：wu
*************************************************************/
void defTheUnit::on_listUnit_clicked(const QModelIndex &index)
{
    int listIndex = index.row();
    if(listIndex<unitList.length())
    {
        unitDeal = unitList.at(listIndex);

        ui->editUnitName->setText(unitDeal.name);
        ui->spinUnitCycle->setValue(unitDeal.cycleCount);
        ui->editUnitDes->setText(unitDeal.unitDes);

        //clear
        for(uint16_t i=ui->tableAction->rowCount();i>0;i--)
        {
            ui->tableAction->removeRow(i-1);
        }

        //append
        for(int i=0;i<unitDeal.actTest.length();i++)
        {
            int row=ui->tableAction->rowCount();
            ui->tableAction->setRowCount(row+1);

            ui->tableAction->selectRow(row);
            ui->tableAction->setItem(row,Col_Name,new QTableWidgetItem(unitDeal.actTest.at(i).actName));
            ui->tableAction->setItem(row,Col_Str,new QTableWidgetItem(unitDeal.actTest.at(i).actStr));

            refreshPropertiesParam(i,unitDeal.actTest.at(i));
        }
    }
}

/*************************************************************
/函数功能：列表右键
/函数参数：指针
/函数返回：wu
*************************************************************/
void defTheUnit::on_listUnit_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    QMenu *popMenu = new QMenu( this );
    QAction *deleteAct = new QAction(tr("删除"), this);
    QAction *clearAct = new QAction(tr("清空"), this);

    if(userLogin.Permissions == Administrator)
    {
        popMenu->addAction(deleteAct);
        popMenu->addAction(clearAct);
    }

    connect( deleteAct,        SIGNAL(triggered() ), this, SLOT( unitDeleteSlot()) );
    connect( clearAct,        SIGNAL(triggered() ), this, SLOT( unitClearSlot()) );

    popMenu->exec( QCursor::pos() );

    delete popMenu;

}

/*************************************************************
/函数功能：删除测试单元
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::unitDeleteSlot()
{
    QListWidgetItem *item = ui->listUnit->currentItem();
    if( item == NULL )
        return;

    int index = ui->listUnit->currentRow();
    QString delStr = item->text();
    Model_XMLFile xmlFile;
    if(xmlFile.hasUnitInfomation(configPath("unitDefined.xml"),delStr))
    {
        if (QMessageBox::warning(NULL, tr("提示"),tr("确定删除该测试测试单元?"),QMessageBox::Yes | QMessageBox::No,QMessageBox::No) != QMessageBox::Yes )
            return;

        xmlFile.removeUnitXML(configPath("unitDefined.xml"),delStr);

        QListWidgetItem* delitem = ui->listUnit->takeItem(index);
        delete delitem;

        if(index<unitList.length())
            unitList.removeAt(index);
    }
}

/*************************************************************
/函数功能：清空测试单元
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::unitClearSlot()
{
    Model_XMLFile xmlFile;
    if (QMessageBox::warning(NULL, tr("提示"),tr("确定清空该测试单元?"),QMessageBox::Yes | QMessageBox::No,QMessageBox::No) != QMessageBox::Yes )
        return;

    for(int i=0;i<unitList.length();i++)
    {
        xmlFile.removeUnitXML(configPath("unitDefined.xml"),unitList.at(i).name);
    }


    ui->listUnit->clear();
    unitList.clear();
}

/*************************************************************
/函数功能：新建测试单元
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::on_actnew_triggered()
{
    int row=ui->tableAction->rowCount();

    if(row)
    {
        int selKey = QMessageBox::warning(NULL, tr("提示"),tr("是否已经保存当前测试单元?"),QMessageBox::Save | QMessageBox::Yes | QMessageBox::Cancel,QMessageBox::Yes);
        if (selKey == QMessageBox::Cancel )
            return ;
        else if (selKey == QMessageBox::Save )
            on_actSave_triggered();
    }

    //主窗口涉及清楚任务，因此放置上层处理
    for(uint16_t i=row;i>0;i--)
    {
        ui->tableAction->removeRow(i-1);
    }

    //初始化测试单元基本信息
    unitDeal.actTest.clear();
    unitDeal.name = "";
    unitDeal.cycleCount = 2;
    unitDeal.unitDes = "";
    ui->editUnitName->setText(unitDeal.name);
    ui->spinUnitCycle->setValue(unitDeal.cycleCount);
    ui->editUnitDes->setText(unitDeal.unitDes);
}

void defTheUnit::on_actHelp_triggered()
{
    QString pdfPath=QDir::currentPath()+"/Unit编辑窗口使用说明.pdf";
    if(QDesktopServices::openUrl(QUrl::fromLocalFile(pdfPath))==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("该运行目录下无《Unit编辑窗口使用说明.pdf》文档！"));
    }
}


