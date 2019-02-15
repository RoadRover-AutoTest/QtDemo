#include "cfgeditunit.h"
#include "ui_cfgeditunit.h"

CfgEditUnit::CfgEditUnit(tUnit *editUnit ,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CfgEditUnit)
{
    ui->setupUi(this);

    theUnit = editUnit;

    WidgetInit();

}

CfgEditUnit::~CfgEditUnit()
{
    delete ui;
}

/*************************************************************
/函数功能：初始化界面窗口
/函数参数：无
/函数返回：无
*************************************************************/
void CfgEditUnit::WidgetInit()
{
    //DataSource：tab标签栏优化
    ui->tabWidget_DataSource->tabBar()->setStyle(new CustomTabStyle);

    //Changed：变动窗体不显示
    on_checkBox_timeChanged_clicked(false);
    on_checkBox_voltChanged_clicked(false);

    //添加list默认按键列表
    EditActList();

    //根据现有的测试填充列表
    theUnitWidgetInit();

    //属性页显示：
    ui->tabWidget_ActConfig->setCurrentWidget(ui->tab_properties);
    ui->tabWidget_DataSource->setCurrentWidget(ui->tab_Waittime);

    //tree窗口展开:
    ui->treeWidget_EditAct->expandAll();
    ui->treeWidget_listAct->expandAll();

}

/*************************************************************
/函数功能：初始化测试单元
/函数参数：无
/函数返回：无
*************************************************************/
void CfgEditUnit::theUnitWidgetInit()
{
    for(int i=0;i<theUnit->actTest.length();i++)
    {
        tAction theAct = theUnit->actTest.at(i);
        EditActAdd(&theAct);
    }

    ui->lineEdit_UnitName->setText(theUnit->name);
    ui->spinBox_UnitCycle->setValue(theUnit->cycleCount);
    ui->textEdit_UnitDes->setText(theUnit->unitDes);
}

/*************************************************************
/函数功能：点击时间变动选项
/函数参数：checked
/函数返回：无
*************************************************************/
void CfgEditUnit::on_checkBox_timeChanged_clicked(bool checked)
{
    ui->groupBox_timeChange->setVisible(checked);
}

/*************************************************************
/函数功能：点击电压变动选项
/函数参数：checked
/函数返回：无
*************************************************************/
void CfgEditUnit::on_checkBox_voltChanged_clicked(bool checked)
{
    ui->groupBox_voltChange->setVisible(checked);
}

/*  this is treeWidget_listAct deal*/
/*************************************************************
/函数功能：编辑ACTlist列表：将硬件资源罗列
/函数参数：无
/函数返回：无
*************************************************************/
void CfgEditUnit::EditActList()
{
    Model_XMLFile xmlRead;
    QString showList;

    keyList.clear();

    xmlRead.readKeyInfoXML(WorkItem,&keyList);

    if(keyList.isEmpty()==false)
    {
        for(int i=0;i<keyList.length();i++)
        {
            if(keyList.at(i).isUse)
            {
                showList = "KEY"+QString::number(i+1)+":"+keyList.at(i).name;
                ui->treeWidget_listAct->topLevelItem(topKey)->addChild(new QTreeWidgetItem(QStringList()<<showList));
            }
        }
    }
}

/*************************************************************
/函数功能：listAct列表右键菜单处理
/函数参数：点击位置
/函数返回：无
*************************************************************/
void CfgEditUnit::on_treeWidget_listAct_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *curItem=ui->treeWidget_listAct->itemAt(pos);  //获取当前被点击的节点

    if(curItem == NULL)
        return;//在空白位置右击

    QMenu *popMenu =new QMenu(this);
    QAction *Add = new QAction(tr("Add to Action"), this);
    QAction *Open = new QAction(tr("Open Script"), this);

    if(curItem == ui->treeWidget_listAct->topLevelItem(topScript))
    {
        popMenu->addAction( Open );
    }
    else if(curItem != ui->treeWidget_listAct->topLevelItem(topKey))
    {
        popMenu->addAction( Add );
    }

    connect( Add, SIGNAL(triggered() ), this, SLOT( ActListAddSlot()) );
    connect( Open, SIGNAL(triggered() ), this, SLOT( ActListOpenSlot()) );

    popMenu->exec(QCursor::pos());

    delete popMenu;
    delete Add;
    delete Open;
}

/*************************************************************
/函数功能：listAct菜单点击打开槽函数：打开脚本所在文件夹，遍历脚本文件
/函数参数：无
/函数返回：无
*************************************************************/
void CfgEditUnit::ActListOpenSlot()
{
    QTreeWidgetItem * item = ui->treeWidget_listAct->currentItem();
    if( item == NULL )
        return;

    //选择脚本文件夹
    QString dirPath=QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("选择脚本文件"),QDir::currentPath()));

    if(!dirPath.isEmpty())
    {
        //判断路径是否存在
        QDir dir(dirPath);
        if((!dir.exists())||(dirPath.isEmpty()))
            return;
        else
            dir.setFilter(QDir::Files | QDir::NoSymLinks);

        scripePath = dirPath;

        QFileInfoList list = dir.entryInfoList();
        if(list.count() <= 0)
            return;

        for(int i=0;i<list.length();i++)
        {
            QString filename=list.at(i).fileName();

            //只取bat文件
            if(filename.contains(".bat") || filename.contains(".BAT"))
            {
                ui->treeWidget_listAct->topLevelItem(topScript)->addChild(new QTreeWidgetItem(QStringList()<<list.at(i).baseName()));
            }
        }
    }
}

/*************************************************************
/函数功能：判断当前项目的动作类型
/函数参数：项目
/函数返回：类型
************************************************************
tType CfgEditUnit::judgetActionType(QTreeWidgetItem *item)
{
    if(item->parent() == ui->treeWidget_listAct->topLevelItem(topKey))
        return KEY;
    else
        return SCRIPT;
}
*/
/*************************************************************
/函数功能：点击添加槽函数：将选中的动作添加到编辑树中
/函数参数：无
/函数返回：无
*************************************************************/
void CfgEditUnit::ActListAddSlot()
{
    QTreeWidgetItem * item = ui->treeWidget_listAct->currentItem();
    if( item == NULL )
        return;
#if any
    //Add theUnit:
    tAction act;

    act.actType = judgetActionType(item);
    act.tDelTime = 0;
    if(act.actType ==KEY)
    {
        act.actName = item->text(col)+":OFF";
        act.actStr = item->text(col)+":off";
    }
    else
    {
        act.actName = item->text(col);
        act.actStr = scripePath + act.actName;
    }

    act.chkType = 0x00;
    initNoCheckParam(&act);
    EditActAdd(&act);
    //将动作添加到测试单元中
    theUnit->actTest.append(act);
#endif
}

/*this is treeWidget_EditAct Deal    动作处理依赖：theUnit*/

/*************************************************************
/函数功能：动作编辑列表添加，同时添加动作到theUnit
/函数参数：测试动作
/函数返回：无
*************************************************************/
void CfgEditUnit::EditActAdd(tAction *act)
{
    //Add Action:
    ui->treeWidget_EditAct->addTopLevelItem(new QTreeWidgetItem(QStringList()<<act->actName));

    //添加动作子属性：
    int row = ui->treeWidget_EditAct->topLevelItemCount();
    QTreeWidgetItem *actItem = ui->treeWidget_EditAct->topLevelItem(row-1);

    //Add ActStr:
    AddTreeNode(actItem,0,QStringList()<<"actStr"<<act->actStr);

    //Add tDelTime:
    //AddTreeNode(actItem,0,QStringList()<<"tDelTime"<<QString::number(act->tDelTime));

    //Add chkType;
    /*if(act->chkType & CHKCurrent)
    {
        QString string;
        if(act->cJudge == GELE)
            string = QString::number(act->current1)+"-"+QString::number(act->current2);
        else
            string = getRangeJudge(act->cJudge)+QString::number(act->current1);
        AddTreeNode(actItem,0,QStringList()<<"CheckCurrent"<<string);
    }

    if(act->chkType & CHKVlot)
    {
        QString string;
        if(act->vJudge == GELE)
            string = QString::number(act->volt1)+"-"+QString::number(act->volt2);
        else
            string = getRangeJudge(act->vJudge)+QString::number(act->volt1);
        AddTreeNode(actItem,0,QStringList()<<"CheckVolt"<<string);
    }

    if(act->chkType & CHKSound)
    {
        AddTreeNode(actItem,0,QStringList()<<"CheckSound"<<getSoundJudge(act->tSound));
    }

    if(act->chkType & CHKScript)
    {
        AddTreeNode(actItem,0,QStringList()<<"CheckScript"<<act->logContains);
    }

    if(act->chkType & CHKRES)
    {
        //AddTreeNode(actItem,"CheckRes","");
    }*/
}

/*************************************************************
/函数功能：点击测试编辑窗口：同步在下方显示单元属性及相应配置项
/函数参数：点击位置
/函数返回：无
*************************************************************/
void CfgEditUnit::on_treeWidget_EditAct_clicked(const QModelIndex &index)
{
    QTreeWidgetItem * item = ui->treeWidget_EditAct->currentItem();
    if( item == NULL )
        return ;

    int itemIndex;

    if(item->parent() == NULL)
        itemIndex = index.row();//指top index 若是子节点将为0（任意树的子节点）
    else
        itemIndex = ui->treeWidget_EditAct->indexOfTopLevelItem(item->parent());//若是子节点定位父节点偏移量来确定当前点击

    if(theUnit->actTest.length()>itemIndex)
    {
#if any
        //属性这一栏主要是查看，修改的话只处理按键的开关：
        tAction theAct = theUnit->actTest.at(itemIndex);
        EditActProperties(&theAct);

        ui->lineEdit_waittime->setText(QString::number(theAct.tDelTime));

        if(theAct.chkType & CHKCurrent)
        {
            ui->groupBox_cur->setChecked(true);
            ui->comboBox_cJudge->setCurrentIndex(theAct.cJudge);
            ui->lineEdit_curMin->setText(QString::number(theAct.current1));
            ui->lineEdit_curMax->setText(QString::number(theAct.current2));
        }
        else
        {
            ui->groupBox_cur->setChecked(false);
            ui->comboBox_cJudge->setCurrentIndex(0);
            ui->lineEdit_curMin->setText("");
            ui->lineEdit_curMax->setText("");
        }

        if(theAct.chkType & CHKVlot)
        {
            ui->groupBox_volt->setChecked(true);
            ui->comboBox_vJudge->setCurrentIndex(theAct.vJudge);
            ui->lineEdit_voltMin->setText(QString::number(theAct.volt1));
            ui->lineEdit_voltMax->setText(QString::number(theAct.volt2));
        }
        else
        {
            ui->groupBox_volt->setChecked(false);
            ui->comboBox_vJudge->setCurrentIndex(0);
            ui->lineEdit_voltMin->setText("");
            ui->lineEdit_voltMax->setText("");
        }

        if(theAct.chkType & CHKSound)
        {
            ui->groupBox_sound->setChecked(true);
            ui->comboBox_soundChanged->setCurrentIndex(theAct.tSound);
        }
        else
        {
            ui->groupBox_sound->setChecked(false);
            ui->comboBox_soundChanged->setCurrentIndex(0);
        }

        if(theAct.chkType & CHKScript)
        {
            ui->groupBox_script->setChecked(true);
            ui->lineEdit_scriptString->setText(theAct.logContains);
        }
        else
        {
            ui->groupBox_script->setChecked(false);
            ui->lineEdit_scriptString->setText("OK (1 test)");
        }

        if(theAct.chkType & CHKRES)
        {
            //AddTreeNode(actItem,"CheckRes","");
        }
#endif
    }
}

/*************************************************************
/函数功能：点击测试编辑窗口右键菜单
/函数参数：点击位置
/函数返回：无
*************************************************************/
void CfgEditUnit::on_treeWidget_EditAct_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *curItem=ui->treeWidget_EditAct->itemAt(pos);  //获取当前被点击的节点

    if(curItem == NULL)
        return;//在空白位置右击

    QMenu *popMenu =new QMenu(this);
    QAction *Add = new QAction(tr("Add to Action"), this);
    QAction *Open = new QAction(tr("Open Script"), this);



    popMenu->addAction( Open );
    popMenu->addAction( Add );


    //connect( Add, SIGNAL(triggered() ), this, SLOT( ActListAddSlot()) );
    //connect( Open, SIGNAL(triggered() ), this, SLOT( ActListOpenSlot()) );

    popMenu->exec(QCursor::pos());

    delete popMenu;
    delete Add;
    delete Open;
}


/*  this is treeWidget_properties deal  属性编辑窗口：按键信息可修改/脚本信息为固定显示*/
/*************************************************************
/函数功能：编辑动作属性
/函数参数：动作
/函数返回：无
*************************************************************/
void CfgEditUnit::EditActProperties(tAction *act)
{
    ui->treeWidget_properties->clear();
    if(act->actStr.contains("KEY"))
    {
        ui->treeWidget_properties->setColumnCount(3);
        ui->pushButton_Changed->setEnabled(true);
        ui->treeWidget_properties->addTopLevelItem(new QTreeWidgetItem(QStringList()<<"Name:"<<act->actName));
        ui->treeWidget_properties->addTopLevelItem(new QTreeWidgetItem(QStringList()<<"actStr:"<<act->actStr));

        EditKeyProperties(act->actStr);

        ui->treeWidget_properties->setColumnWidth(1,ui->treeWidget_properties->width()-180);
        ui->treeWidget_properties->setColumnWidth(2,50);
    }
    else
    {
        ui->treeWidget_properties->setColumnCount(3);
        ui->pushButton_Changed->setEnabled(false);
        ui->treeWidget_properties->addTopLevelItem(new QTreeWidgetItem(QStringList()<<"Name:"<<act->actName));
        ui->treeWidget_properties->addTopLevelItem(new QTreeWidgetItem(QStringList()<<"actStr:"<<act->actStr));
    }
    ui->treeWidget_properties->expandAll();
    eAct = *act;
}

/*************************************************************
/函数功能：编辑按键属性及配置操作组件
/函数参数：无
/函数返回：无
//备注：动作属性为按键时有效
*************************************************************/
void CfgEditUnit::EditKeyProperties(QString actStr)
{
    //显示按键属性：
    ui->treeWidget_properties->addTopLevelItem(new QTreeWidgetItem(QStringList()<<"KEY"));
    int row = ui->treeWidget_properties->topLevelItemCount();
    QTreeWidgetItem *keyItem = ui->treeWidget_properties->topLevelItem(row-1);

    //选择 按键：
    QComboBox *kInf = new QComboBox;
    for(int i=0;i<keyList.length();i++)
    {
        if(keyList.at(i).isUse)
            kInf->addItem("KEY"+QString::number(i+1)+":"+ keyList.at(i).name);
    }
    ui->treeWidget_properties->setItemWidget(keyItem,1,kInf);
    connect(kInf,SIGNAL(currentIndexChanged(QString)),this,SLOT(comboBox_CurrentChanged(QString)));

    //编辑 按键：
    QPushButton *editKey = new QPushButton(QIcon(":/test/edit.png"),"Edit");
    ui->treeWidget_properties->setItemWidget(keyItem,2,editKey);
    connect(editKey,SIGNAL(clicked(bool)),this,SLOT(pushButton_Edit_clicked()));

    //按键 信息：
    int keyNum = actStr.mid(3,1).toUInt();//取按键号
    if(keyNum<=keyList.length())
    {
        keyControl theKey = keyList.at(keyNum-1);

        //按键 开关：
        if((theKey.type != Can1_1)||(theKey.type != Can1_1))
        {
            AddTreeNode(keyItem,0,QStringList()<<"OnOff:");
            QCheckBox *onoff = new QCheckBox;
            if(actStr.contains(":on"))
                onoff->setChecked(true);
            ui->treeWidget_properties->setItemWidget(keyItem->child(0),1,onoff);
            connect(onoff,SIGNAL(clicked(bool)),this,SLOT(checkBox_KeyOnOff(bool)));
        }

        //按键 描述：
        AddTreeNode(keyItem,0,QStringList()<<"des:"<<theKey.des);
    }
}

/*************************************************************
/函数功能：点击按键编辑按钮
/函数参数：无
/函数返回：无
//备注：动作属性为按键时有效
*************************************************************/
void CfgEditUnit::pushButton_Edit_clicked()
{
    ResHardware *hardCfg=new ResHardware;

    hardCfg->exec();

    //更新列表：
    ui->treeWidget_listAct->topLevelItem(topKey)->takeChildren();
    EditActList();

#if 0
    //判断按键属性是否变更：
    if(eAct.actType == KEY)
    {
        QStringList kList = eAct.actName.split(":");
        QString keyName;
        int keyIndex;

        if(kList > 1)
        {
            keyIndex = kList.at(0).right(1).toUInt();
            keyName = kList.at(1);

            if(keyList.length()>keyIndex)
            {

            }
        }
    }
#endif

    delete hardCfg;
}

/*************************************************************
/函数功能：选择按键
/函数参数：按键字符串
/函数返回：无
//备注：动作属性为按键时有效
*************************************************************/
void CfgEditUnit::comboBox_CurrentChanged(QString string)
{
    int row = ui->treeWidget_properties->topLevelItemCount();
    QTreeWidgetItem *keyItem = ui->treeWidget_properties->topLevelItem(row-1);

    int keyNum = string.mid(3,1).toUInt();//取按键号
    if(keyNum<=keyList.length())
    {
        keyControl theKey = keyList.at(keyNum-1);
        keyItem->child(keyItem->childCount()-1)->setText(1,theKey.des);

        QString onoffStr = ui->treeWidget_properties->topLevelItem(0)->text(1).split(":").last();
        ui->treeWidget_properties->topLevelItem(0)->setText(1,string+":"+onoffStr);
        ui->treeWidget_properties->topLevelItem(1)->setText(1,string+":"+onoffStr.toLower());
    }
}

/*************************************************************
/函数功能：设置按键开关
/函数参数：开/关
/函数返回：无
//备注：动作属性为按键时有效
*************************************************************/
void CfgEditUnit::checkBox_KeyOnOff(bool chicked)
{
    if(chicked)
    {
        QString keyStr = ui->treeWidget_properties->topLevelItem(0)->text(1).remove(":OFF");
        ui->treeWidget_properties->topLevelItem(0)->setText(1,keyStr+":ON");
        ui->treeWidget_properties->topLevelItem(1)->setText(1,keyStr+":on");
    }
    else
    {
        QString keyStr = ui->treeWidget_properties->topLevelItem(0)->text(1).remove(":ON");
        ui->treeWidget_properties->topLevelItem(0)->setText(1,keyStr+":OFF");
        ui->treeWidget_properties->topLevelItem(1)->setText(1,keyStr+":off");
    }
}

/*************************************************************
/函数功能：点击查看测试单元
/函数参数：无
/函数返回：无
*************************************************************/
void CfgEditUnit::on_pushButton_unitLook_clicked()
{
    CfgLookUnit lookUnit(theUnit);

    lookUnit.exec();
}


