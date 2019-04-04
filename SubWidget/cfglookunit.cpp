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
    Cld2_Info,
    Cld2_Check
}actRowType_e;





CfgLookUnit::CfgLookUnit(tUnit *unit,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CfgLookUnit)
{
    ui->setupUi(this);

    uint8_t showFlag=0;


    AddTreeRoot("Unit:",unit->name);
    int row = ui->treeWidget->topLevelItemCount();
    QTreeWidgetItem * topitem=ui->treeWidget->topLevelItem(row-1);
    AddTreeNode(topitem,showFlag,QStringList()<<tr("次数")<< toStr(unit->cycleCount) );
    AddTreeNode(topitem,showFlag,QStringList()<<tr("描述")<<unit->unitDes);

    row = ui->treeWidget->topLevelItemCount();
    for(int i=0;i<unit->actTest.length();i++)
    {
        tAction theAct = unit->actTest.at(i);

        //动作作为top节点显示：
        //AddTreeRoot("Action:",theAct.actName);
        //QTreeWidgetItem * item=ui->treeWidget->topLevelItem(i+row);

        //将动作最为测试单元子节点显示
        AddTreeNode(topitem,showFlag,QStringList()<<tr("动作")+toStr(i+1)<<theAct.actName);
        QTreeWidgetItem * item=topitem->child(i+Cld1_Act);//2为添加动作前添加了cycle和描述节点

        AddTreeNode(item,showFlag,QStringList()<<tr("执行")<<theAct.actStr);
        AddTreeNode(item,showFlag,QStringList()<<tr("等待时间")<<toStr(theAct.timeDeal.wait));
        AddTreeNode(item,showFlag,QStringList()<<tr("检测时间")<<toStr(theAct.timeDeal.check));
        AddTreeNode(item,showFlag,QStringList()<<tr("超时时间")<<toStr(theAct.timeDeal.end));

        //cout << "colinfo:"<<theAct.infoFlag;//打印，通过LOG查看
        cout << "errorDeal:"<<theAct.errorDeal;

        //采集信息显示
        if(theAct.colInfoList.isEmpty()==false)
            AddTreeNode(item,0,QStringList()<<tr("信息采集"));

        for(int j=0;j<theAct.colInfoList.length();j++)
        {
            QTreeWidgetItem * infoitem = item->child(Cld2_Info);
            AddTreeNode(infoitem,showFlag,QStringList()<<""<<theAct.colInfoList.at(j));//信息采集参数不显示
        }

        //检测信息显示
        if(theAct.checkDeal.isEmpty()==false)
            AddTreeNode(item,0,QStringList()<<tr("检测"));

        for(int j=0;j<theAct.checkDeal.length();j++)
        {
            checkParam chkShow = theAct.checkDeal.at(j);
            QTreeWidgetItem * chkitem = item->child(Cld2_Check);

            switch(chkShow.check)
            {
            case CHKCurrent:
                if(chkShow.range!=GELE)
                    AddTreeNode(chkitem,showFlag,QStringList()<<tr("电流")<<getRangeJudge(chkShow.range)+QString::number(chkShow.min));
                break;
            case CHKVlot:
                if(chkShow.range!=GELE)
                    AddTreeNode(chkitem,showFlag,QStringList()<<tr("电压")<<getRangeJudge(chkShow.range)+QString::number(chkShow.min));
                break;
            case CHKSound:
                AddTreeNode(chkitem,showFlag,QStringList()<<tr("声音")<<getSoundJudge(chkShow.sound));
                break;
            case CHKScript:
                AddTreeNode(chkitem,showFlag,QStringList()<<tr("脚本")<<chkShow.logContains);
                break;
            case CHKInterface:
                if(chkShow.infoCompare == MemoryCompare)
                    AddTreeNode(chkitem,showFlag,QStringList()<<tr("界面")<<getCompareType(chkShow.infoCompare)+"    "+chkShow.comTarget);
                else
                    AddTreeNode(chkitem,showFlag,QStringList()<<tr("界面")<<getCompareType(chkShow.infoCompare));
                break;
            case CHKADBPIC:
                if(chkShow.infoCompare == MemoryCompare)
                    AddTreeNode(chkitem,showFlag,QStringList()<<tr("图片")<<getCompareType(chkShow.infoCompare)+"    "+chkShow.comTarget);
                else
                    AddTreeNode(chkitem,showFlag,QStringList()<<tr("图片")<<getCompareType(chkShow.infoCompare));
                break;
            case CHKRES:break;
            }
        }

        //变动信息显示
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


