#include "retablewidget.h"



reTableWidget::reTableWidget(QWidget *parent)
    : QTableWidget(parent)
{

}



reTableWidget::~reTableWidget()
{

}

/*************************************************************
/函数功能：添加行数据
/函数参数：list
/函数返回：wu
*************************************************************/
void reTableWidget::appendTableWidget(QStringList info)
{
    if(info.length() != 3)
    {
        cout << "error:length!=3";
        return ;
    }

    int row=this->rowCount();
    this->setRowCount(row+1);

    QTableWidgetItem *unitName = new QTableWidgetItem(info.at(0));
    //unitName->setCheckState(Qt::Checked);
    this->setItem(row,seqColName,unitName);

    QSpinBox* spinBox3 = new QSpinBox();
    spinBox3->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    spinBox3->setMinimum(1);
    spinBox3->setMaximum(65535);
    spinBox3->setFocusPolicy(Qt::ClickFocus);
    spinBox3->setProperty("row", row);
    spinBox3->setProperty("col", 3);
    spinBox3->setValue(info.at(1).toUInt());
    this->setCellWidget(row, seqColCycle, spinBox3);
    connect(spinBox3, SIGNAL(valueChanged(int)), this, SLOT(onSpbColumnChanged(int)));

    this->setItem(row,seqColDes,new QTableWidgetItem(info.at(2)));

    this->setColumnWidth(0,200);
}

/*************************************************************
/函数功能：测试次数改变
/函数参数：值
/函数返回：wu
*************************************************************/
void reTableWidget::onSpbColumnChanged(int value)
{
    QSpinBox * senderObj=qobject_cast<QSpinBox *>(sender());
    if(senderObj == 0)
        return;

    QModelIndex index =this->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));

    if((index.row()>=0)&&(index.row()<theSeqList.length()))
    {
        tUnit testUnit=theSeqList.at(index.row());

        testUnit.cycleCount=value;
        theSeqList.replace(index.row(),testUnit);
    }
}

/*************************************************************
/函数功能：所有测试单元
/函数参数：无
/函数返回：unitlist
*************************************************************/
QList <tUnit>*reTableWidget::unitAll()
{
    return (&theSeqList);
}

/*************************************************************
/函数功能：重写鼠标点击操作.
/函数参数：事件：处理右键
/函数返回：wu
*************************************************************/
void reTableWidget::mousePressEvent(QMouseEvent *event)
{
    //确保左键拖拽.
    if (event->button() == Qt::RightButton)
    {
        QMenu *popMenu = new QMenu( this );
        QAction *deleteCase = new QAction(tr("删除"), this);
        QAction *clearCases = new QAction(tr("清空"), this);
        QAction *RunCuetomCase = new QAction(tr("Run CustomCase"), this);
        QAction *RunALLCases = new QAction(tr("Run AllCase"), this);
        QAction *upCase = new QAction(tr("上移"), this);
        QAction *downCases = new QAction(tr("下移"), this);
        QAction *LookCase = new QAction(tr("查看"), this);
        QAction *EditCase = new QAction(tr("编辑"), this);
        QAction *addMenu = new QAction("添加");

        if(userLogin.Permissions == Administrator)
        {
            popMenu->addAction(addMenu);
            popMenu->addAction( LookCase );
            popMenu->addAction( EditCase );
           // popMenu->addAction( RunCuetomCase );
           // popMenu->addAction( RunALLCases );
            //popMenu->addSeparator();
            popMenu->addSeparator();
            popMenu->addAction( upCase );
            popMenu->addAction( downCases );
            popMenu->addAction( deleteCase );
            popMenu->addAction( clearCases );
        }
        else
        {
            popMenu->addAction( LookCase );
            popMenu->addAction( EditCase );
        }



        connect( addMenu,          SIGNAL(triggered() ), this, SLOT( AddTestCasetoListSlot()) );
        connect( deleteCase,        SIGNAL(triggered() ), this, SLOT( deleteSeqfromTableSlot()) );
        connect( clearCases,        SIGNAL(triggered() ), this, SLOT( clearSeqfromTableSlot()) );
        connect( RunCuetomCase,     SIGNAL(triggered() ), this, SLOT( RunSeqCustomSlot()) );
        connect( RunALLCases,       SIGNAL(triggered() ), this, SLOT( RunSeqAllSlot()) );
        connect( upCase,            SIGNAL(triggered() ), this, SLOT( UpTheUnitSlot()) );
        connect( downCases,         SIGNAL(triggered() ), this, SLOT( DownTheUnitSlot()) );
        connect( LookCase,          SIGNAL(triggered() ), this, SLOT( LookTheUnitSlot()) );
        connect( EditCase,          SIGNAL(triggered(bool)), this, SLOT( EditTheUnitSlot()) );

        popMenu->exec( QCursor::pos() );

        delete popMenu;
    }

    //保留原QListWidget部件的鼠标点击操作.
    QTableWidget::mousePressEvent(event);
}

/*************************************************************
/函数功能：获取选择范围
/函数参数：
/函数返回：选择行号列表
*************************************************************/
QList <int> reTableWidget::selTableRanges()
{
    QList <int> vecItemIndex;//保存选中行的索引
    QItemSelectionModel *selections = this->selectionModel(); //返回当前的选择模式
    QModelIndexList selectedsList = selections->selectedIndexes(); //返回所有选定的模型项目索引列表

    for (int i = 0; i < selectedsList.count(); i++)
    {
        //vecItemIndex.push_back(selectedsList.at(i).row());
        //cout <<selectedsList.at(i).row();
        vecItemIndex.append(selectedsList.at(i).row());
    }
    //std::sort(vecItemIndex.begin(), vecItemIndex.end());
    //vecItemIndex.erase(std::unique(vecItemIndex.begin(), vecItemIndex.end()), vecItemIndex.end());
    return vecItemIndex;
}

/*************************************************************
/函数功能：删除列表当前选中项
/函数参数：
/函数返回：无
*************************************************************/
void reTableWidget::deleteSeqfromTableSlot()
{
    if (QMessageBox::warning(NULL, tr("提示"),tr("确定删除该测试单元?"),QMessageBox::Yes | QMessageBox::No,QMessageBox::No) != QMessageBox::Yes )
        return;

    QList <int> itemIndex = selTableRanges();
    int index=0;
    int maxSel=0;

    while(itemIndex.length())
    {
        //查找所选行号中最大行号
        maxSel =itemIndex.at(index);
        for(int i=1;i<itemIndex.length();i++)
        {
            if(maxSel<itemIndex.at(i))
            {
                maxSel = itemIndex.at(i);
                index = i;
            }
        }

        //删除该行号的数据
        this->removeRow(maxSel);
        if((maxSel>=0)&&(maxSel<theSeqList.length()))
        {
            theSeqList.removeAt(maxSel);
        }
        //移除，重新查找，知道无数据选中
        itemIndex.removeAt(index);
        index = 0;
    }
}

/*************************************************************
/函数功能：清空测试单元列表
/函数参数：
/函数返回：无
*************************************************************/
void reTableWidget::clearSeqfromTableSlot()
{
    if (QMessageBox::warning(NULL, tr("提示"),tr("确定清空当前测试流?"),QMessageBox::Yes | QMessageBox::No,QMessageBox::No) != QMessageBox::Yes )
        return;

    //主窗口涉及清楚任务，因此放置上层处理
    int row=this->rowCount();
    for(uint16_t i=row;i>0;i--)
    {
        this->removeRow(i-1);
    }

    theSeqList.clear();
}

/*************************************************************
/函数功能：删除列表当前选中项
/函数参数：
/函数返回：无
*************************************************************/
void reTableWidget::RunSeqCustomSlot()
{
    QTableWidgetItem * item = this->currentItem();
    if( item == NULL )
        return;

    int curIndex = this->row(item);
    RunSeqCustom(curIndex);

}

/*************************************************************
/函数功能：运行测试单元列表
/函数参数：
/函数返回：无
*************************************************************/
void reTableWidget::RunSeqAllSlot()
{
    QTableWidgetItem * item = this->currentItem();
    if( item == NULL )
        return;

    RunSeqAll();
}

/*************************************************************
/函数功能：新建序列文件
/函数参数：无
/函数返回：无
*************************************************************/
void reTableWidget::onNewSequenceSlot()
{
    if(theSeqList.isEmpty()==false)
    {
        clearSeqfromTableSlot();
    }

    //更新测试路径
    if(theSeqList.isEmpty())
        seqSavePath.clear();
}

/*************************************************************
/函数功能：打开序列文件
/函数参数：无
/函数返回：无
*************************************************************/
void reTableWidget::onOpenSequenceSlot()
{
    QString path=QFileDialog::getOpenFileName(this , tr("打开") , "" , tr("Text Files(*.xml)"));

    if(path.isEmpty()==false)
    {
        if(theSeqList.isEmpty() == false)
        {
            clearSeqfromTableSlot();//清除当前测试序列
        }

        Model_XMLFile xmlRead;
        int len = theSeqList.length();
        xmlRead.readSequenceXML(path,theSeqList);

        for(int i=len;i<theSeqList.length();i++)
        {
            QStringList unitInfo;

            unitInfo.append(theSeqList.at(i).name);
            unitInfo.append(toStr(theSeqList.at(i).cycleCount));
            unitInfo.append(theSeqList.at(i).unitDes);

            appendTableWidget(unitInfo);
        }
        seqSavePath = path;
    }
}

/*************************************************************
/函数功能：获得序列文件名
/函数参数：无
/函数返回：文件名
*************************************************************/
QString reTableWidget::getSequenceFileName()
{
    return seqSavePath.split("/").last().remove(".xml");
}

/*************************************************************
/函数功能：保存测试列表
/函数参数：
/函数返回：无
*************************************************************/
void reTableWidget::onSaveSequenceSlot()
{
    QString path;
    if(seqSavePath.isEmpty())
        path = QFileDialog::getSaveFileName(this , tr("保存") , "." , tr("Text Files(*.xml)"));
    else
        path = seqSavePath;

    if(!path.isEmpty())
        SaveTheSeq_Deal(path);
}

/*************************************************************
/函数功能：另存为：选择文件保存
/函数参数：无
/函数返回：无
*************************************************************/
void reTableWidget::onSaveAsSequenceSlot()
{
    QString path = QFileDialog::getSaveFileName(this , tr("另存为") , "." , tr("Text Files(*.xml)"));

    if(!path.isEmpty())
        SaveTheSeq_Deal(path);
}

/*************************************************************
/函数功能：序列文件保存
/函数参数：路径
/函数返回：无
*************************************************************/
void reTableWidget::SaveTheSeq_Deal(QString path)
{
    Model_XMLFile xmlSave;
    tUnit unit;
    QStringList unitList;

    xmlSave.createSequenceXML(path);

    for(int i=0;i<theSeqList.length();i++)
    {
        unit=theSeqList.at(i);
        unitList.clear();
        unitList<<unit.name<<QString::number(unit.cycleCount)<<unit.unitDes;//"this is test"

        for(int j=0;j<unit.actTest.length();j++)
        {
            xmlSave.appendSequenceXML(path,unitList,unit.actTest.at(j));
        }
    }
    seqSavePath = path;
}

/*************************************************************
/函数功能：查看当前测试单元
/函数参数：无
/函数返回：无
*************************************************************/
void reTableWidget::LookTheUnitSlot()
{
    QTableWidgetItem * item = this->currentItem();
    if( item == NULL )
        return;

    int curIndex = this->row(item);
    tUnit unit=theSeqList.at(curIndex);
    CfgLookUnit lookUnit(&unit);

    lookUnit.exec();
}

/*************************************************************
/函数功能：编辑当前测试单元
/函数参数：无
/函数返回：无
*************************************************************/
void reTableWidget::EditTheUnitSlot()
{
    QTableWidgetItem * item = this->currentItem();
    if( item == NULL )
        return;
    int curIndex = this->row(item);
    tUnit unitDeal = theSeqList.at(curIndex);
    defUnit = new defTheUnit(&unitDeal);
    connect(defUnit ,SIGNAL(applyTheUnit(tUnit)),this,SLOT(replaceDefineUnitSlot(tUnit)));
    defUnit->show();
}

/*************************************************************
/函数功能：上移
/函数参数：
/函数返回：无
*************************************************************/
void reTableWidget::UpTheUnitSlot()
{
    QTableWidgetItem * item = this->currentItem();
    if( item == NULL )
        return;

    int curIndex = this->row(item);

    moveRow(curIndex, curIndex-1 );
}

/*************************************************************
/函数功能：保存测试列表
/函数参数：
/函数返回：无
*************************************************************/
void reTableWidget::DownTheUnitSlot()
{
    QTableWidgetItem * item = this->currentItem();
    if( item == NULL )
        return;

    int curIndex = this->row(item);
    moveRow(curIndex, curIndex+1 );//2
}

/*************************************************************
/函数功能：移动行：表格显示及序列排序
/函数参数：
/函数返回：无
*************************************************************/
void reTableWidget::moveRow(int nFrom, int nTo )
{
    if( this == NULL )
        return;
    this->setFocus();

    if( nFrom == nTo )
        return;
    if( nFrom < 0 || nTo < 0 )
        return;
    int nRowCount = this->rowCount();
    if( nFrom >= nRowCount  || nTo > nRowCount )
        return;
    if( nFrom >= theSeqList.length()  || nTo > theSeqList.length() )
        return;

    //交换序列
    tUnit testUnit=theSeqList.at(nFrom);
    tUnit swopUnit=theSeqList.at(nTo);

    theSeqList.replace(nFrom,swopUnit);
    theSeqList.replace(nTo,testUnit);

    //交换表格显示表格显示
    if( nTo < nFrom )
        nFrom++;
    else
        nTo++;
    this->insertRow( nTo );

    int nCol = this->columnCount();

    for( int i=0; i<nCol; i++ )
    {
        if(i != seqColCycle)
            this->setItem( nTo, i, this->takeItem( nFrom , i ) );
        else
            this->setCellWidget( nTo, i, (QSpinBox *)this->cellWidget(nFrom , i) );
    }

    if( nFrom < nTo  )
        nTo--;

    this->removeRow( nFrom );
    this->selectRow( nTo );
}

/*************************************************************
/函数功能：一键生成脚本测试1：带ACC/BAT
/函数参数：
/函数返回：无
*************************************************************/
void reTableWidget::ScriptCase1Slot()
{
#if 0
    int len = theSeqList.length();

    //配置脚本运行环境
    CfgSParam *sparam=new CfgSParam(&theSeqList);

    if(sparam->exec() == QDialog::Accepted)
    {
        for(int i=len;i<theSeqList.length();i++)
        {
            QStringList unitInfo;

            unitInfo.append(theSeqList.at(i).name);
            unitInfo.append(toStr(theSeqList.at(i).cycleCount));
            unitInfo.append(theSeqList.at(i).unitDes);

            appendTableWidget(unitInfo);
        }
    }
    delete sparam;
#endif
}

/*************************************************************
/函数功能：添加测试单元
/函数参数：
/函数返回：无
*************************************************************/
void reTableWidget::AddTestCasetoListSlot()
{
    tUnit unitDeal;
    unitDeal.name.clear();
    unitDeal.actTest.clear();
    defUnit = new defTheUnit(&unitDeal);
    connect(defUnit ,SIGNAL(applyTheUnit(tUnit)),this,SLOT(applyDefineUnitSlot(tUnit)));
    defUnit->show();
}

/*************************************************************
/函数功能：应用定义的测试单元
/函数参数：测试单元：批量时将数据拆分
/函数返回：无
*************************************************************/
void reTableWidget::applyDefineUnitSlot(tUnit unit)
{
    int fileIndex=0;
    bool isOver=false;
    QString reString;

    tUnit curUnit;
    curUnit.name = unit.name;
    curUnit.cycleCount = unit.cycleCount;
    curUnit.unitDes = unit.unitDes;

continueAppendList:
    curUnit.actTest.clear();
    for(int i=0;i<unit.actTest.length();i++)
    {
        tAction actDeal = unit.actTest.at(i);

        if((actDeal.actFlag == ACT_KEY)
                ||((actDeal.actFlag == ACT_SCRIPT)&&((actDeal.actStr.endsWith(".bat"))||(actDeal.actStr.endsWith(".BAT")))))
        {
            curUnit.actTest.append(actDeal);
        }
        else
        {
            QString dirPath = actDeal.actStr;
            //判断路径是否存在
            QDir dir(dirPath);
            if((!dir.exists())||(dirPath.isEmpty()))
            {
                QMessageBox::warning(NULL, tr("提示"), tr("未选择脚本文件或不存在该路径！"));
                return;
            }
            else
                dir.setFilter(QDir::Files | QDir::NoSymLinks);

            QFileInfoList list = dir.entryInfoList();

            if(fileIndex<list.length())
            {
                QString filename=list.at(fileIndex).absoluteFilePath();//.baseName();//QFileInfo
                QString Script = filename.split('/').last().remove(".bat");

                //只取bat文件
                if(filename.contains(".bat") || filename.contains(".BAT"))
                {
                curUnit.name =unit.name + "_"+Script;
                actDeal.actName = Script;
                actDeal.actStr += "\\"+Script+".bat";
                curUnit.actTest.append(actDeal);
                }
                else
                {
                    //非bat文件，跳过
                    if(++fileIndex<list.length())
                        goto continueAppendList;
                    else
                        return;
                }
            }
            if(++fileIndex>=list.length())
                isOver=true;
        }
    }

    //查询，若序列中已存在，跳过添加，若不存在添加测试单元
    int index;
    for(index=0;index<theSeqList.length();index++)
    {
        //测试序列中已存在将跳过应用
        if(curUnit.name == theSeqList.at(index).name)
        {
            reString+="\n"+curUnit.name;
            break;
        }
    }
    if(index>=theSeqList.length())
    {
        theSeqList.append(curUnit);
        QStringList unitInfo;
        int len = theSeqList.length()-1;

        unitInfo.append(theSeqList.at(len).name);
        unitInfo.append(toStr(theSeqList.at(len).cycleCount));
        unitInfo.append(theSeqList.at(len).unitDes);

        appendTableWidget(unitInfo);
    }


    if((fileIndex)&&(!isOver))
        goto continueAppendList;
    else if(reString.isEmpty()==false)
    {
        QMessageBox::warning(NULL, tr("提示"), tr("存在重复测试单元添加，如下所示：")+reString);
    }
}

/*************************************************************
/函数功能：替换测试单元
/函数参数：编辑后的测试单元
/函数返回：无
*************************************************************/
void reTableWidget::replaceDefineUnitSlot(tUnit unit)
{
    int index;
    for(index=0;index<theSeqList.length();index++)
    {
        if(theSeqList.at(index).name == unit.name)
        {
            theSeqList.replace(index,unit);
            break;
        }
    }
    if(index==theSeqList.length())
        QMessageBox::warning(NULL, tr("提示"), tr("编辑后未找到对应的测试单元，编辑失效！"));
}
