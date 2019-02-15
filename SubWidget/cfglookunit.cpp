#include "cfglookunit.h"
#include "ui_cfglookunit.h"

CfgLookUnit::CfgLookUnit(tUnit *unit,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CfgLookUnit)
{
    ui->setupUi(this);

    lookUnit = unit;

    AddTreeRoot("Unit:",lookUnit->name);
    int row = ui->treeWidget->topLevelItemCount();
    QTreeWidgetItem * item=ui->treeWidget->topLevelItem(row-1);
    AddTreeNode(item,0,QStringList()<<"Cycle"<< toStr(lookUnit->cycleCount) );
    AddTreeNode(item,0,QStringList()<<"Description"<<lookUnit->unitDes);

    row = ui->treeWidget->topLevelItemCount();
    for(int i=0;i<lookUnit->actTest.length();i++)
    {
        tAction theAct = lookUnit->actTest.at(i);

        AddTreeRoot("Action:",theAct.actName);

        QTreeWidgetItem * item=ui->treeWidget->topLevelItem(i+row);

        AddTreeNode(item,0,QStringList()<<"actStr"<<theAct.actStr);
        AddTreeNode(item,0,QStringList()<<"waittime"<<toStr(theAct.timeDeal.wait));
        AddTreeNode(item,0,QStringList()<<"checktime"<<toStr(theAct.timeDeal.check));
        AddTreeNode(item,0,QStringList()<<"endtime"<<toStr(theAct.timeDeal.end));

        for(int j=0;j<theAct.checkDeal.length();j++)
        {
            checkParam chkShow = theAct.checkDeal.at(j);

            switch(chkShow.check)
            {
            case CHKCurrent:
                if(chkShow.range!=GELE)
                    AddTreeNode(item,0,QStringList()<<"chkCurrent"<<getRangeJudge(chkShow.range)+QString::number(chkShow.min));
                break;
            case CHKVlot:break;
            case CHKSound:break;
            case CHKScript:
                AddTreeNode(item,0,QStringList()<<"chkScript"<<chkShow.logContains);
                break;
            case CHKMEMORY:
                AddTreeNode(item,0,QStringList()<<"chkMemory"<<toStr(chkShow.isMemory));
                break;
            case CHKADBPIC:break;
            case CHKRES:break;
            }
        }

        for(int j=0;j<theAct.changedDeal.length();j++)
        {
            changedParam cngShow = theAct.changedDeal.at(j);

            switch(cngShow.changed)
            {
            case WaitTime:
                AddTreeNode(item,0,QStringList()<<"change"<<"waittime");
                break;
            case BatVolt:
                AddTreeNode(item,0,QStringList()<<"change"<<"waittime");
                break;
            }
            AddTreeNode(item,0,QStringList()<<"dir"<<toStr(cngShow.dir));
            AddTreeNode(item,0,QStringList()<<"min"<<toStr(cngShow.min));
            AddTreeNode(item,0,QStringList()<<"max"<<toStr(cngShow.max));
            AddTreeNode(item,0,QStringList()<<"step"<<toStr(cngShow.step));

        }
    }

    ui->treeWidget->setColumnWidth(0,200);
    ui->treeWidget->expandAll();

}

CfgLookUnit::~CfgLookUnit()
{
    delete ui;
}

QTreeWidgetItem * CfgLookUnit::AddTreeRoot(QString name,QString desc)
{
    QTreeWidgetItem * item=new QTreeWidgetItem(QStringList()<<name<<desc);
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    ui->treeWidget->addTopLevelItem(item);
    return item;
}
