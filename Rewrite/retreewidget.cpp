#include "retreewidget.h"



reTreeWidget::reTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{

}



reTreeWidget::~reTreeWidget()
{

}
/*************************************************************
/函数功能：treeWidget初始化：PC接口树组件
/函数参数：无
/函数返回：无
*************************************************************/
void reTreeWidget::interfaceTreeWidgetInit()
{
    QTreeWidgetItem *itemUart = this->topLevelItem(topUart);
    QTreeWidgetItem *itemDrive = this->topLevelItem(topDEV);

    //UART
    QComboBox *UartCom = new QComboBox;
    this->setItemWidget(itemUart->child(uartCOM),colItem,UartCom);

    QComboBox *UartBaud = new QComboBox;
    QStringList baudList;
    baudList << "115200" << "57600" << "38400" << "19200" << "14400" << "9600" << "4800";
    UartBaud->addItems(baudList);
    this->setItemWidget(itemUart->child(uartBAUD),colItem,UartBaud);

    //DEV
    QComboBox *devNum1Com = new QComboBox;
    this->setItemWidget(itemDrive->child(devNum1),colItem,devNum1Com);

#if 0
    QPushButton *WIFIConnect = new QPushButton("连接");
    WIFIConnect->setCheckable(true);
    WIFIConnect->setEnabled(false);
    this->setItemWidget(itemDrive->child(devWIFI)->child(devCONNECT),colItem,WIFIConnect);
    connect(WIFIConnect,SIGNAL(clicked(bool)),this,SLOT(wifiConnectDeal(bool)));


    QComboBox *DEVCOM = new QComboBox;
    this->setItemWidget(itemDrive->child(devUSB),colItem,DEVCOM);
    connect(DEVCOM,SIGNAL(activated(int)),this,SLOT(clickedUSBPort(int)));
    initWinAPIDeal();
    refreshUSBPort();
    clickedUSBPort(0);//初始化设备序列号：为第一个设备的序列号

    //CAN
    QComboBox *CANType = new QComboBox;
    baudList.clear();
    baudList <<  "HCAN";
    CANType->addItems(baudList);
    this->setItemWidget(itemCAN->child(canType),colItem,CANType);

    QComboBox *CANBaud = new QComboBox;
    baudList.clear();
    baudList << "500K" << "10K" << "20K" << "33.3K" << "40K" << "50K" << "80K" << "83.3K" << "100K" << "125K" << "200K" << "250K" << "400K" << "800K" << "1M";
    CANBaud->addItems(baudList);
    this->setItemWidget(itemCAN->child(canBaud),colItem,CANBaud);

#endif

    connect(this,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(itemChangedCheckedState(QTreeWidgetItem*,int)));

    isSetChecked=false;
}

#if 0
/*************************************************************
/函数功能：无线连接
/函数参数：无
/函数返回：无
*************************************************************/
void reTreeWidget::wifiConnectDeal(bool checked)
{
    QPushButton *WIFIButton = (QPushButton *)this->itemWidget(this->topLevelItem(topDEV)->child(devWIFI)->child(devCONNECT),colItem);
    QString ipStr = this->topLevelItem(topDEV)->child(devWIFI)->child(devIP)->text(colItem);

    wifiIsConnect(checked,ipStr);
    if(checked)
    {
        WIFIButton->setText("断开");
    }
    else
    {
        WIFIButton->setText("连接");
    }
}
#endif

/*************************************************************
/函数功能：treeWidget项目改变
/函数参数：改变的项目，第几列
/函数返回：无
/备注：选择测试项,并切换显示使能
*************************************************************/
void reTreeWidget::itemChangedCheckedState(QTreeWidgetItem *curItem ,int column)
{
    if(column!=colItem)
        return ;

    if(isSetChecked)
    {
        isSetChecked=false;
        return ;
    }

    QTreeWidgetItem *topItem = curItem->parent();
    int index = this->indexOfTopLevelItem(topItem);
    if(index == topDEV)
    {
        if(curItem->checkState(column) == Qt::Checked)
            devUseState(true);
        else
            devUseState(false);
    }
    else if(index == topUart)
    {
        if(curItem->checkState(column) == Qt::Checked)
        {
            QString com=getQComboBoxCurrentTest(topItem->child(uartCOM));
            QString baud=getQComboBoxCurrentTest(topItem->child(uartBAUD));
            uartOpen(com,baud);

            this->itemWidget(topItem->child(0),colItem)->setEnabled(false);
            this->itemWidget(topItem->child(1),colItem)->setEnabled(false);
        }
        else
        {
            uartClose();

            this->itemWidget(topItem->child(0),colItem)->setEnabled(true);
            this->itemWidget(topItem->child(1),colItem)->setEnabled(true);
        }
    }
}

/*************************************************************
/函数功能：刷新串口端口
/函数参数：端口列表
/函数返回：无
*************************************************************/
void reTreeWidget::refreshUartCOM(QStringList comList)
{
    QComboBox *com=(QComboBox *)this->itemWidget(this->topLevelItem(topUart)->child(uartCOM),colItem);
    QString curStr=com->currentText();

    com->clear();
    com->addItems(comList);
    com->setCurrentText(curStr);
}

/*************************************************************
/函数功能：刷新设备
/函数参数：设备列表
/函数返回：无
*************************************************************/
void reTreeWidget::refreshDevNum1(QStringList devList)
{
    QComboBox *com=(QComboBox *)this->itemWidget(this->topLevelItem(topDEV)->child(devNum1),colItem);
    QString curStr=com->currentText();

    com->clear();
    com->addItems(devList);
    com->setCurrentText(curStr);
}


/*************************************************************
/函数功能：获得comboBox的当前选中项字符串：用于所有该组件的项
/函数参数：当前项目
/函数返回：解析的字符串
*************************************************************/
QString reTreeWidget::getQComboBoxCurrentTest(QTreeWidgetItem *curItem)
{
    QComboBox *comboBox=(QComboBox *)this->itemWidget(curItem,colItem);

    return comboBox->currentText();
}

/*************************************************************
/函数功能：设置选择状态：//只有串口或CAN对应启动修改状态：用于当主界面不满足启动条件时，将选择状态恢复
/函数参数：当前项目 使能、失能
/函数返回：无
*************************************************************/
void reTreeWidget::setCheckedState(int topItem,bool checked)
{
    QTreeWidgetItem *item=this->topLevelItem(topItem);

    isSetChecked=true;//在设置的前面处理，设置时会进入槽函数

    if(checked)
        item->child(uartUSE)->setCheckState(colItem,Qt::Checked);
    else
        item->child(uartUSE)->setCheckState(colItem,Qt::Unchecked);


}
#if 1
/*************************************************************
/函数功能：重绘项目标签项：颜色加重
/函数参数：当前项目
/函数返回：解析的字符串
*************************************************************/
void reTreeWidget::drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    if (index.parent() == QModelIndex())
    {
        const QColor c = option.palette.color(QPalette::Dark);
        painter->fillRect(option.rect, c);
        opt.palette.setColor(QPalette::AlternateBase, c);
    }
    QTreeWidget::drawRow(painter, opt, index);
    QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
    painter->save();
    painter->setPen(QPen(color));
    painter->drawLine(opt.rect.x(), opt.rect.bottom(), opt.rect.right(), opt.rect.bottom());
    painter->restore();
}
#endif

#if 0
else if(index == topCAN)
{
    if(curItem->checkState(column) == Qt::Checked)
    {
        QString type=getQComboBoxCurrentTest(topItem->child(canType));
        QString baud=getQComboBoxCurrentTest(topItem->child(canBaud));
        canOpen(type,baud);

        this->itemWidget(topItem->child(0),colItem)->setEnabled(false);
        this->itemWidget(topItem->child(1),colItem)->setEnabled(false);
    }
    else
    {
        canClose();

        this->itemWidget(topItem->child(0),colItem)->setEnabled(true);
        this->itemWidget(topItem->child(1),colItem)->setEnabled(true);
    }
}
/*this is WinApi option*/
/*************************************************************
/函数功能：初始化API处理
/函数参数：无
/函数返回：无
*************************************************************/
void reTreeWidget::initWinAPIDeal()
{
    WinDeal = new Model_WinApi;

    connect(WinDeal,SIGNAL(USBhDevNotify()),this,SLOT(refreshUSBPort()));
}

/*************************************************************
/函数功能：释放API处理
/函数参数：无
/函数返回：无
*************************************************************/
void reTreeWidget::deleteWinAPIDeal()
{
    delete WinDeal;
}

/*************************************************************
/函数功能：刷新USB端口
/函数参数：端口列表
/函数返回：无
*************************************************************/
void reTreeWidget::refreshUSBPort()
{
    QTreeWidgetItem *itemDrive = this->topLevelItem(topDEV);
    QComboBox *port=(QComboBox *)this->itemWidget(itemDrive->child(devUSB),colItem);
    QString curStr=port->currentText();

    DevsList.clear();
    DevsList=WinDeal->winAPIGetUSBInfo();

    port->clear();
    for(int i=0;i<DevsList.length();i++)
    {
        port->addItem(DevsList.at(i).Port);
    }

    port->setCurrentText(curStr);
}

/*************************************************************
/函数功能：点击USB端口
/函数参数：index
/函数返回：无
*************************************************************/
void reTreeWidget::clickedUSBPort(int index)
{
    QTreeWidgetItem *itemDrive = this->topLevelItem(topDEV);

    if(index<DevsList.length())
        itemDrive->child(devNum)->setText(colItem,DevsList.at(index).dNumber);
}
#endif
