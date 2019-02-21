#include "deftheunit.h"
#include "ui_deftheunit.h"

defTheUnit::defTheUnit(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::defTheUnit)
{
    ui->setupUi(this);

    //tab标签栏优化
    ui->tabProperties->tabBar()->setStyle(new CustomTabStyle);

    //初始化按键列表
    Model_XMLFile xmlRead;
    QList <keyControl> keyList;
    xmlRead.readKeyInfoXML(WorkItem,&keyList);

    for(int i=0;i<keyList.length();i++)
    {
        if(keyList.at(i).isUse)
            ui->comboKeyList->addItem("KEY"+QString::number(i+1)+":"+keyList.at(i).name);
    }

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

/*************************************************************
/函数功能：按键动作添加槽函数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::keyActionSlot()
{
    tAction kAction;

    kAction.actName = "";
    kAction.actStr =  "";

    kAction.timeDeal.wait = 0;
    kAction.timeDeal.check = 0;
    kAction.timeDeal.end = 0;

    kAction.infoFlag=0;
    kAction.checkDeal.clear();
    kAction.changedDeal.clear();

    appendTableAction(1,kAction);
}

/*************************************************************
/函数功能：脚本动作添加槽函数
/函数参数：无
/函数返回：wu
*************************************************************/
void defTheUnit::scriptActionSlot()
{
    tAction sAction;

    sAction.actName = "";
    sAction.actStr = "";
    sAction.timeDeal.wait = 0;
    sAction.timeDeal.check = 0;
    sAction.timeDeal.end = 0;

    sAction.infoFlag=0;
    sAction.checkDeal.clear();
    sAction.changedDeal.clear();

    checkParam checkScript;
    initNullChkParam(&checkScript);
    checkScript.check = CHKScript;
    checkScript.logContains = "OK (1 test)";
    sAction.checkDeal.append(checkScript);

    appendTableAction(0,sAction);
}

void defTheUnit::appendTableAction(bool flag,tAction act)
{
    int row=ui->tableAction->rowCount();
    ui->tableAction->setRowCount(row+1);

    if(flag)
        ui->tableAction->setItem(row,Col_Action,new QTableWidgetItem(QIcon(":/test/current.png"),"KEY"));
    else
        ui->tableAction->setItem(row,Col_Action,new QTableWidgetItem(QIcon(":/test/current.png"),"Script"));

    ui->tableAction->setItem(row,Col_Name,new QTableWidgetItem(act.actName));
    ui->tableAction->setItem(row,Col_Str,new QTableWidgetItem(act.actStr));

    ui->tableAction->setItem(row,Col_WaitTime,new QTableWidgetItem(toStr(act.timeDeal.wait)));
    ui->tableAction->setItem(row,Col_ChkTime,new QTableWidgetItem(toStr(act.timeDeal.check)));
    ui->tableAction->setItem(row,Col_OverTime,new QTableWidgetItem(toStr(act.timeDeal.end)));
    ui->tableAction->item(row,Col_WaitTime)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);//居中显示
    ui->tableAction->item(row,Col_ChkTime)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->tableAction->item(row,Col_OverTime)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);


    QString chkString;
    for(int i=0;i<act.checkDeal.length();i++)
    {
        chkString += getCheckType(act.checkDeal.at(i).check)+";";
    }
    ui->tableAction->setItem(row,Col_Check,new QTableWidgetItem(chkString));

    unitDeal.actTest.append(act);
}

void defTheUnit::on_tableAction_clicked(const QModelIndex &index)
{
    if(index.row()>=unitDeal.actTest.length())
        return ;

    tAction selAction = unitDeal.actTest.at(index.row());

    QString typeStr = ui->tableAction->item(index.row(),Col_Action)->text();

    if(typeStr == "KEY")
        initPropertiesParam(1,selAction);
    else
        initPropertiesParam(0,selAction);

}

void defTheUnit::getTableActionSelRanges(int *selrow)
{
    //因设置了单选，因此只取一个参数即可
    QList <QTableWidgetSelectionRange>ranges = ui->tableAction->selectedRanges();

    for(int i=0;i<ranges.count();i++)
    {
        *selrow = ranges.at(i).topRow();
    }
}


void defTheUnit::initPropertiesParam(bool type,tAction act)
{
    ui->editActName->setText(act.actName);
    if(type)
    {
        QString actStr = act.actStr;
        int keyNum = getKeyNumber(actStr);
        ui->stackedWidget->setCurrentWidget(ui->pageKey);

        if(keyNum != -1)
        {
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
    {
        ui->stackedWidget->setCurrentWidget(ui->pageScript);
        ui->editFilePath->setText(act.actStr);
    }
}

void defTheUnit::on_editActName_editingFinished()
{
    int bot;
    getTableActionSelRanges(&bot);
    cout<<bot;
}
