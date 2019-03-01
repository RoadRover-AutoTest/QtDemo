#include "cfglookunit.h"
#include "ui_cfglookunit.h"

typedef enum
{
    Cld1_CYC,
    Cld1_DES,
    Cld1_Act
}cld1Type_e;


typedef enum
{
    Cld2_Str,
    Cld2_Wait,
    Cld2_ChkTime,
    Cld2_Over,
    Cld2_Check
}actRowType_e;





CfgLookUnit::CfgLookUnit(tUnit *unit,bool isEdit,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CfgLookUnit)
{
    ui->setupUi(this);
    uint8_t showFlag=0;

    lookUnit = unit;
    //if(isEdit)
    //    showFlag|=0x01;

    AddTreeRoot("Unit:",lookUnit->name);
    int row = ui->treeWidget->topLevelItemCount();
    QTreeWidgetItem * topitem=ui->treeWidget->topLevelItem(row-1);
    AddTreeNode(topitem,showFlag,QStringList()<<"Cycle"<< toStr(lookUnit->cycleCount) );
    AddTreeNode(topitem,showFlag,QStringList()<<"Description"<<lookUnit->unitDes);

    row = ui->treeWidget->topLevelItemCount();
    for(int i=0;i<lookUnit->actTest.length();i++)
    {
        tAction theAct = lookUnit->actTest.at(i);

        //动作作为top节点显示：
        //AddTreeRoot("Action:",theAct.actName);
        //QTreeWidgetItem * item=ui->treeWidget->topLevelItem(i+row);

        //将动作最为测试单元子节点显示
        AddTreeNode(topitem,showFlag,QStringList()<<"Action"<<theAct.actName);
        QTreeWidgetItem * item=topitem->child(i+Cld1_Act);//2为添加动作前添加了cycle和描述节点

        AddTreeNode(item,showFlag,QStringList()<<"actStr"<<theAct.actStr);
        AddTreeNode(item,showFlag,QStringList()<<"waittime"<<toStr(theAct.timeDeal.wait));
        AddTreeNode(item,showFlag,QStringList()<<"checktime"<<toStr(theAct.timeDeal.check));
        AddTreeNode(item,showFlag,QStringList()<<"endtime"<<toStr(theAct.timeDeal.end));
        //AddTreeNode(item,showFlag,QStringList()<<"colinfo"<<toStr(theAct.infoFlag));//信息采集参数不显示
        cout << "colinfo:"<<theAct.infoFlag;//打印，通过LOG查看

        if(theAct.checkDeal.isEmpty()==false)
            AddTreeNode(item,0,QStringList()<<"Check");

        for(int j=0;j<theAct.checkDeal.length();j++)
        {
            checkParam chkShow = theAct.checkDeal.at(j);
            QTreeWidgetItem * chkitem = item->child(Cld2_Check);;

            switch(chkShow.check)
            {
            case CHKCurrent:
                if(chkShow.range!=GELE)
                    AddTreeNode(chkitem,showFlag,QStringList()<<"Current"<<getRangeJudge(chkShow.range)+QString::number(chkShow.min));
                break;
            case CHKVlot:
                if(chkShow.range!=GELE)
                    AddTreeNode(chkitem,showFlag,QStringList()<<"Volt"<<getRangeJudge(chkShow.range)+QString::number(chkShow.min));
                break;
            case CHKSound:
                AddTreeNode(chkitem,showFlag,QStringList()<<"Sound"<<getSoundJudge(chkShow.sound));
                break;
            case CHKScript:
                AddTreeNode(chkitem,showFlag,QStringList()<<"Script"<<chkShow.logContains);
                break;
            case CHKInterface:
                AddTreeNode(chkitem,showFlag,QStringList()<<"Interface"<<getCompareType(chkShow.infoCompare));
                break;
            case CHKADBPIC:
                AddTreeNode(chkitem,showFlag,QStringList()<<"ADBPicture"<<getCompareType(chkShow.infoCompare));
                break;
            case CHKRES:break;
            }
        }

        for(int j=0;j<theAct.changedDeal.length();j++)
        {
            changedParam cngShow = theAct.changedDeal.at(j);
            QTreeWidgetItem * cngitem;//=topitem->child(i+2);
            switch(cngShow.changed)
            {
            case WaitTime:
                cngitem = item->child(Cld2_Wait);//在等待时间上添加支节点
                break;
            case BatVolt:
                cngitem = item->child(Cld2_Wait);
                break;
            }
            AddTreeNode(cngitem,showFlag,QStringList()<<"dir"<<toStr(cngShow.dir));
            AddTreeNode(cngitem,showFlag,QStringList()<<"min"<<toStr(cngShow.min));
            AddTreeNode(cngitem,showFlag,QStringList()<<"max"<<toStr(cngShow.max));
            AddTreeNode(cngitem,showFlag,QStringList()<<"step"<<toStr(cngShow.step));
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
    //item->setFlags(item->flags()|Qt::ItemIsEditable);
    ui->treeWidget->addTopLevelItem(item);
    return item;
}

void CfgLookUnit::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{

}
