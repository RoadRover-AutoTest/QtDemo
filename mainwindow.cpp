#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initMainWindow();

    initUartParam();

    initProcessDeal();

    timer1SID=startTimer(1000);
    initkeyList();

    isRunning=false;
    testState=overtest;
    isDelayReport=false;
}

MainWindow::~MainWindow()
{
    killTimer(timer1SID);
    deleteProcessDeal();
    deleteUartParam();
    delete chartDeal;
    delete ui;
}




/*---------------------------------------this is windowDeal option-----------------------------------------*/
/*************************************************************
/函数功能：初始化主界面窗口
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::initMainWindow()
{
    QWidget* p = takeCentralWidget();
    if(p)
        delete p;

    setDockNestingEnabled(true);

    //记录所有的dock指针
    m_docks.append(ui->dockWidgetLeft);
    m_docks.append(ui->dockWidgetBottom);
    m_docks.append(ui->dockWidgetSequence);
    m_docks.append(ui->dockWidgetRuning);
    m_docks.append(ui->dockWidgetShow);

    reRankDockWidget();

    //初始化显示界面
    setIsRunInterface(false);

    //初始化接口树参数
    ui->treeWidget->interfaceTreeWidgetInit();
    ui->treeWidget->refreshUartCOM(UARTDeal->PortList());
    //ui->treeWidget->expandAll();


    ui->textBrowser_EXEShow->setOpenExternalLinks(true);//设置添加超链接

    chartDeal =new ChartWidget;
    ui->dockWidgetShow->setWidget(chartDeal);

    ui->toolBar_Fun->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    connect(ui->actionrank,SIGNAL(triggered(bool)),this,SLOT(reRankDockWidget()));

    //取配置信息
    Model_iniSetting InfoINI;
    WorkItem = InfoINI.ReadIni_item();
    WorkCurrent = InfoINI.ReadIni_WorkCurrent();
    WorkFrequency = InfoINI.ReadIni_WorkFrequency();
    ReportCreat = InfoINI.ReadIni_ReportCreat();

}

/*************************************************************
/函数功能：重新排序窗口
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::reRankDockWidget()
{
    //移除窗口
    for(int i=0;i<dockMax;++i)
    {
        removeDockWidget(m_docks[i]);
    }

    //排序
    addDockWidget(Qt::LeftDockWidgetArea,ui->dockWidgetLeft);
    splitDockWidget(ui->dockWidgetLeft,ui->dockWidgetBottom,Qt::Vertical);
    splitDockWidget(ui->dockWidgetLeft,ui->dockWidgetSequence,Qt::Horizontal);
    tabifyDockWidget(ui->dockWidgetSequence,ui->dockWidgetRuning);
    tabifyDockWidget(ui->dockWidgetRuning,ui->dockWidgetShow);

    //显示
    for(int i=0;i<dockMax;++i)
    {
        m_docks[i]->show();
    }
}

/*************************************************************
/函数功能：是否切换到运行界面
/函数参数：true：是  false：否
/函数返回：无
*************************************************************/
void MainWindow::setIsRunInterface(bool IsRun)
{
    if(IsRun)
    {
        ui->dockWidgetBottom->setVisible(true);
        ui->dockWidgetRuning->raise();   //显示运行窗口
        ui->acttest->setText("Ending");
        ui->acttest->setIcon(QIcon(":/Title/actEnding.png"));
        ui->acttest->setChecked(IsRun);
    }
    else
    {
        ui->dockWidgetBottom->setVisible(true);//初始化界面：
        ui->acttest->setText("Running");
        ui->acttest->setIcon(QIcon(":/Title/actRunning.png"));
        ui->acttest->setChecked(IsRun);
    }
}

/*************************************************************
/函数功能：主窗口关闭事件
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::closeEvent(QCloseEvent *event)
{
    if(getTestRunState())
    {
        if(QMessageBox::information(NULL, "Exit", "正在测试，是否退出软件?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::Yes)//Upgrading
        {
            endTheFlow();
            event->accept();
        }
        else
            event->ignore();
    }
    else
        event->accept();
}

/*************************************************************
/函数功能：定时器事件，处理测试
/函数参数：事件
/函数返回：无
*************************************************************/
void MainWindow::timerEvent(QTimerEvent *event)
{
    //1S定时
    if(event->timerId()==timer1SID)
    {
        //显示当前时间
        QDateTime current_date_time =QDateTime::currentDateTime();
        QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss ddd");//.zzz
        ui->statusBar->showMessage(current_date);

        //刷新uart COM
        ui->treeWidget->refreshUartCOM(UARTDeal->PortList());

        //1S获取下物理参数:any:Error
        if((getTestRunState())||(testState!=overtest))
        {
            chkParamFromHardware(0x00);
        }

        //实时扫描设备
        if((!getTestRunState())&&(testState==overtest))
        {
            if(proList.isEmpty())
                proList.append(ADBDevs);
            //cout;
        }
    }
    //串口定时器处理：
    else if(event->timerId()==timerUartID)
    {
        timerUartIDDeal();
    }
    //进程定时器处理+显示处理：
    else if(event->timerId() == timerProID)
    {
        timerProIDDeal();

        //显示执行
        if(ShowList.isEmpty() == false)
        {
            ui->textBrowser_EXEShow->append(ShowList.first());
            ShowList.removeFirst();
        }
    }
    //测试定时器处理
    else if(event->timerId()==timerTestID)
    {
        timerTestIDDeal();
    }
}

/*************************************************************
/函数功能：硬件资源
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::on_actHard_triggered()
{
    ResHardware *hardCfg=new ResHardware;

    hardCfg->exec();
    initkeyList();

    delete hardCfg;
}

/*************************************************************
/函数功能：生成报告
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::on_actReport_triggered()
{
    DiaReport *reportDeal = new DiaReport;

    reportDeal->exec();
    delete reportDeal;
}

/*************************************************************
/函数功能：工具配置
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::on_actATtool_triggered()
{
    toolConfig *ATConfig = new toolConfig();

    ATConfig->exec();

    delete ATConfig;
}

/*************************************************************
/函数功能：帮助
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::on_actHelp_triggered()
{
    QString pdfPath=QDir::currentPath()+"/自动化测试系统方案.pdf";
    if(QDesktopServices::openUrl(QUrl::fromLocalFile(pdfPath))==false)
    {
        QMessageBox::warning(NULL, QString("提示"), QString("该运行目录下无说明文档！"));
    }

}

/*************************************************************
/函数功能：关于
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::on_about_triggered()
{
    QMessageBox::information(NULL, "About", "自动化测试系统 V1.01\n"
                                            "日期：2019.01.17\n"
                                            "版权：roadrover\n"
                                            "反馈邮箱：lishuhui@roadrover.cn");
}

/*************************************************************
/函数功能：点击执行测试/结束测试
/函数参数：点击状态
/函数返回：无
*************************************************************/
void MainWindow::on_acttest_triggered(bool checked)
{
    if(UartConnectStatus())
    {
        if(checked)
        {
            QList <tUnit>*testList = ui->tableSequence->unitAll();
            if(testList->isEmpty()==false)
                startTheFlow(testList);
            else
                ui->textBrowser_EXEShow->append("启动测试失败：测试序列不存在数据，请先添加测试序列！");
        }
        else
            endTheFlow();
    }

    //处理后状态不一致，代表处理失败，按键状态恢复
    if(checked != getTestRunState())
    {
        ui->acttest->setChecked(!checked);
    }
}




/*---------------------------------------this is treeWidget option-----------------------------------------*/
/*************************************************************
/函数功能：串口打开槽函数
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::on_treeWidget_uartOpen(const QString &com, const QString &baud)
{
    UartOpen(com,baud);
}

/*************************************************************
/函数功能：串口关闭槽函数
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::on_treeWidget_uartClose()
{
    UartClose();
}

/*************************************************************
/函数功能：CAN打开槽函数
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::on_treeWidget_canOpen(const QString &type, const QString &baud)
{
    if(!UartConnectStatus())
    {
        ui->treeWidget->setCheckedState(topCAN,false);
        return ;
    }

    uint16_t Dat_temp=0;
    char SPdata[4];

    SPdata[0]=true;

    if(type=="HCAN")     SPdata[1]=1;
    else if(type=="Scan")SPdata[1]=2;
    else if(type=="Ccan")SPdata[1]=0;

    if(baud=="10K")       Dat_temp=10;
    else if(baud=="20K")  Dat_temp=20;
    else if(baud=="33.3K")Dat_temp=33;
    else if(baud=="40K")  Dat_temp=40;
    else if(baud=="50K")  Dat_temp=50;
    else if(baud=="80K")  Dat_temp=80;
    else if(baud=="83.3K")Dat_temp=83;
    else if(baud=="100K") Dat_temp=100;
    else if(baud=="125K") Dat_temp=125;
    else if(baud=="200K") Dat_temp=200;
    else if(baud=="250K") Dat_temp=250;
    else if(baud=="400K") Dat_temp=400;
    else if(baud=="500K") Dat_temp=500;
    else if(baud=="800K") Dat_temp=800;
    else if(baud=="1M")   Dat_temp=1000;

    SPdata[2]=Dat_temp>>8;
    SPdata[3]=Dat_temp&0x00ff;

    appendTxList(CMDCAN1Channel,SPdata,4,CMD_NEEDACK);
}

/*************************************************************
/函数功能：CAN关闭槽函数
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::on_treeWidget_canClose()
{
    if(!UartConnectStatus())
    {
        ui->treeWidget->setCheckedState(topCAN,false);
        return ;
    }

    char SPdata=false;
    appendTxList(CMDCAN1Channel,&SPdata,1,CMD_NEEDACK);
}

/*************************************************************
/函数功能：设备启用状态
/函数参数：状态
/函数返回：无
//any:待使用
*************************************************************/
void MainWindow::on_treeWidget_devUseState(bool isUse)
{
    cout <<isUse;
}

/*************************************************************
/函数功能：设备刷新
/函数参数：ADB执行进程输出字符串
/函数返回：无
*************************************************************/
void MainWindow::treeWidget_refreshDevNum(QString String)
{
    QStringList devList = String.split("\r\n");

    if(devList.isEmpty()==false)
        devList.removeFirst();

    if(devList.isEmpty()==false)
    {
        for(int i=0;i<devList.length();)
        {
            QString tempString=devList.at(i);
            if(tempString.contains("\tdevice"))
            {
                devList.replace(i,tempString.remove("\tdevice"));
                if(devList.at(i).isEmpty())
                    devList.removeAt(i);
                else
                    i++;
            }
            else
                devList.removeAt(i);
        }
        ui->treeWidget->refreshDevNum1(devList);
    }
}




/*---------------------------------------this is AutoTest & tFlowDeal option---------------------------------------*/
/*************************************************************
/函数功能：测试单元开始执行
/函数参数：执行测试流
/函数返回：无
*************************************************************/
void MainWindow::startTheFlow(QList <tUnit> *testFlow)
{
    if((!isRunning)&&(testState==overtest))
    {
        //设备序列号
        QTreeWidgetItem *topDev = ui->treeWidget->topLevelItem(topDEV);
        if(topDev->child(devUSE)->checkState(colItem)==Qt::Checked)
        {
            QComboBox *comboBox=(QComboBox *)ui->treeWidget->itemWidget(topDev->child(devNum1),colItem);
            devNumber = comboBox->currentText();
        }
        else
        {
            if(QMessageBox::information(NULL, "Warn", "未启用设备序列号，是否继续?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::Yes)//Upgrading
                devNumber.clear();
            else
                return ;
        }

        //提示保存文件：
        if(ui->tableSequence->getSequenceFileName().isEmpty())
        {
            if(QMessageBox::information(NULL, "Warn", "未保存文件，是否继续?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::No)//Upgrading
                return ;
        }

        //创建测试流程：
        tFlowDeal = new Model_tFlow(testFlow);
        connect(tFlowDeal,SIGNAL(unitStartExe(tUnit)),this,SLOT(unitStartExeSlot(tUnit)));
        connect(tFlowDeal,SIGNAL(theUnitResult(bool)),this,SLOT(unitEndExeSlot(bool)));
        connect(tFlowDeal,SIGNAL(flowEndTest()),this,SLOT(onEndTestSlot()));
        connect(tFlowDeal,SIGNAL(keyClicked(QString)),this,SLOT(execKeyClicked(QString)));

        testState=start;
        timerTestID = startTimer(10);
    }
    else
    {
        ui->textBrowser_EXEShow->append("启动测试失败：请检查测试是否正在运行或正在生成报告！");
    }
}

/*************************************************************
/函数功能：测试定时器执行处理
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::timerTestIDDeal()
{
    switch(testState)
    {
    case start:
    {
        isDelayReport=false;
        isRunning = true;
        setIsRunInterface(true);
        testTime = QDateTime::currentDateTime();
        ui->textBrowser_EXEShow->append("启动测试:");
        testState=waitnull;
        break;
    }
    case waitnull:
    {
        if(testString.isEmpty()==false)
        {
            execKeyClicked(testString); //执行按键
            testString.clear();
        }
        break;
    }
    case getprop:
    {
        if(!isPRORunning)
        {
            isProOK=false;
            //机器版本信息:
            ui->textBrowser_EXEShow->append("获取机器版本信息，请稍后... ...");
            appendThePropertiesToFile("clear");

            proList.append("adb -s "+getDevNumber()+SHELLPROP);
        }
        break;
    }
    case report:
    {
        if(!isPRORunning)
        {
            ui->textBrowser_EXEShow->append("正在生成报告，请稍后... ...");
            proList.append(PYTHONREPORT+ResultPath+"/" + ui->tableSequence->getSequenceFileName()+ "/" + testTime.toString("yyyyMMddhhmmss")+"/");
        }
        break;
    }
    case overtest:
    {
        delete tFlowDeal;
        killTimer(timerTestID);
        isRunning=false;
        setIsRunInterface(false);
        break;
    }
    }


}

/*************************************************************
/函数功能：测试进程输出处理
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::testProcessOutputDeal(QString String)
{
    switch(testState)
    {
    case getprop:
    {
        if(String.contains("Out>>"))
        {
            isProOK=true;
            appendThePropertiesToFile(String.remove("Out>>"));
        }
        break;
    }
    default :break;
    }
}

/*************************************************************
/函数功能：测试进程结束处理
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::testProcessOverDeal()
{
    switch(testState)
    {
    case getprop:
    {
        if(isProOK)
        {
            //添加时间
            appendThePropertiesToFile("start_time:"+testTime.toString("yyyy.MM.dd-hh.mm.ss")+"\r\n");
            appendThePropertiesToFile("end_time:"+testTime.toString("yyyy.MM.dd-hh.mm.ss")+"\r\n");
            testState = report;
        }
        break;
    }
    case report:
    {
        ui->textBrowser_EXEShow->append(tr("<html><p><a>报告生成结束，请查找本地对应目录或邮件或</a><a href=\"%1\">点击查阅</a></p></html>\n\n")
                                        .arg("http://192.168.13.96/result/" + ui->tableSequence->getSequenceFileName()+ "/" + testTime.toString("yyyyMMddhhmmss")+"/report.html"));

        testState = overtest;
        break;
    }
    default :break;
    }

    if(isDelayReport)
    {
        testState = getprop;
        isDelayReport=false;
    }
}

/*************************************************************
/函数功能：获取测试运行状态
/函数参数：无
/函数返回：无
*************************************************************/
bool MainWindow::getTestRunState()
{
    return isRunning;
}

/*************************************************************
/函数功能：结束测试
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::endTheFlow()
{
    if(getTestRunState())
    {
        //进程若正在执行，终止：只针对uiautomator，bat中的其他命令继续执行
        if(proSysIsRunning())
            proStopSysUiautomator();

        tFlowDeal->endTheTest();
        isRunning=false;
    }
}

/*************************************************************
/函数功能：测试单元结束执行
/函数参数：无
/函数返回：无  any:测试停止时若机器关，需要将机器开启再进行获取信息，生成报告
*************************************************************/
void MainWindow::onEndTestSlot()
{
    ui->textBrowser_EXEShow->append("结束测试！");
    if(ReportCreat)
    {
        if(isPRORunning)
            isDelayReport=true;
        else
            testState = getprop;
    }
    else
        testState = overtest;
    //cout << testState;
}

/*************************************************************
/函数功能：测试单元开始执行
/函数参数：单元信息
/函数返回：无
*************************************************************/
void MainWindow::unitStartExeSlot(tUnit eUnit)
{
    QDateTime currentTime = QDateTime::currentDateTime();
    int row=ui->tableWidget->rowCount();

    ui->label_ItemName->setText(eUnit.name+"...");

    ui->tableWidget->setRowCount(row+1);
    ui->tableWidget->setItem(row,colUnit,new QTableWidgetItem(QIcon(":/test/current.png"),eUnit.name));
    ui->tableWidget->setItem(row,colAct,new QTableWidgetItem(currentTime.toString("yyyy.MM.dd hh:mm:ss.zzz")));

    ui->tableWidget->setColumnWidth(colUnit,150);
    ui->tableWidget->setColumnWidth(colAct,150);

    savePath = ResultPath+"/" + ui->tableSequence->getSequenceFileName()
                + "/" + testTime.toString("yyyyMMddhhmmss")
                + "/" + eUnit.name
                + "/Loop"+toStr(tFlowDeal->getTheFlowLoop()) ;
    if(appendTheResultToFile("StartTheUnit:"+currentTime.toString("yyyy.MM.dd-hh.mm.ss")+"    name:"+eUnit.name)==false)
    {
        QMessageBox::warning(NULL, QString("Warn"), QString("保存报告路径创建有误，请检查"+savePath+"/report.txt "));
        endTheFlow();
    }
}

/*************************************************************
/函数功能：测试单元结束执行
/函数参数：单元信息
/函数返回：无
*************************************************************/
void MainWindow::unitEndExeSlot(bool exeResult)
{
    int row=ui->tableWidget->rowCount();

    ui->label_ItemName->setText("");

    if(exeResult)
    {
        ui->tableWidget->setItem(row-1,colResult,new QTableWidgetItem(QIcon(":/test/pass.png"),NULL));
    }
    else
    {
        ui->tableWidget->setItem(row-1,colResult,new QTableWidgetItem(QIcon(":/test/fail.png"),NULL));
    }

    ui->tableWidget->setItem(row-1,colStr,new QTableWidgetItem("测试结束"));

    appendTheResultToFile("EndTheUnit:"+QDateTime::currentDateTime().toString("yyyy.MM.dd-hh.mm.ss")+"    result:"+QString::number(exeResult)+"\r\n\r\n");
}

/*************************************************************
/函数功能：测试流执行按键点击
/函数参数：按键
/函数返回：无
/备注：测试流中按键字符串组成：KEY1:Name:on/off || KEY1:Name
/以：分割   制定on/off或无
*************************************************************/
void MainWindow::execKeyClicked(QString key)
{
    QStringList theAct=key.split(':');
    char buf[2]={0};
    if(theAct.isEmpty()==false)
    {
        buf[0] = theAct.at(0).right(1).toInt();//KEY1 取"1"

        if(theAct.length() == 3)
        {
            if(theAct.at(2)=="on")
                buf[1]=true;
            else
                buf[1]=false;
        }
        appendTxList(CMDClickedKey,buf,2,CMD_NEEDACK);
    }
}

/*************************************************************
/函数功能：检测来自硬件的参数
/函数参数：chk：检测的项
/函数返回：无
*************************************************************/
void MainWindow::chkParamFromHardware(uint8_t chk)
{
    char buf=0;

    switch (chk)
    {
    case CHKCurrent:
        appendTxList(CMDWorkCurrent,&buf,1,CMD_NEEDNACK);
        break;
    default:
        break;
    }
}




/*---------------------------------------this is UART option--------------------------------------------*/
/*************************************************************
/函数功能：初始化串口参数
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::initUartParam()
{
    UARTDeal=new Model_UART;
    ackWait=0;
    txList.clear();
}

/*************************************************************
/函数功能：释放串口参数
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::deleteUartParam()
{
    //若串口打开,关串口
    if(UARTDeal->isOpenCurrentUart())
        UartClose();

    delete UARTDeal;
}

/*************************************************************
/函数功能：串口打开
/函数参数：端口，波特率
/函数返回：无
*************************************************************/
void MainWindow::UartOpen(const QString &com, const QString &baud)
{
    UARTDeal->Open(com,baud);

    if(!UARTDeal->isOpenCurrentUart())
        ui->treeWidget->setCheckedState(topUart,false);
    else
    {
        connect(UARTDeal,SIGNAL(RxFrameDat(char,uint8_t,char*)),this,SLOT(UartRxDealSlot(char,uint8_t,char*)));
        connect(UARTDeal,SIGNAL(UartByteArrayBackStage(QByteArray,uartDir,bool)),this,SLOT(UartByteArraySlot(QByteArray,uartDir,bool)));
        connect(UARTDeal,SIGNAL(UartRxAckResault(bool)),this,SLOT(UartRxAckResault(bool)));
        connect(UARTDeal,SIGNAL(UartDisConnect()),this,SLOT(UartDisconnect()));

        timerUartID=startTimer(CmdACKDelay);
    }
}

/*************************************************************
/函数功能：串口关闭
/函数参数：无
/函数返回：无
/备注：断开串口前要断开信号连接，否则将会出现信号多次发送的现象照成帧显示错误；
*************************************************************/
void MainWindow::UartClose()
{
    UARTDeal->disconnect();
    UARTDeal->Close();
    killTimer(timerUartID);
}

/*************************************************************
/函数功能：串口连接状态
/函数参数：无
/函数返回：无
*************************************************************/
bool MainWindow::UartConnectStatus()
{
    bool status=UARTDeal->isOpenCurrentUart();
    if(!status)
        QMessageBox::warning(NULL, QString("Warn"), QString("未打开串口，请处理！"));
    return status;
}

/*************************************************************
/函数功能：处理定时串口ID数据
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::timerUartIDDeal()
{
    if((txList.isEmpty() == false) && (!ackWait))
    {
        uartFrame uartDat=txList.at(0);

        if(uartDat.ack == CMD_NEEDACK)
        {
            ackWait=1;
        }

        //cout << "txList:" <<txList.length();
        UARTDeal->UartTxCmdDeal(uartDat.cmd,uartDat.dat,uartDat.len,uartDat.ack);
        txList.removeFirst();

    }
    else if(ackWait)
    {
        //cout << ackWait;
        if((++ackWait) > (CmdReSendTimer+2))
        {
            ackWait=0;
            txList.clear();
            QMessageBox::warning(NULL, QString("Warn"), QString("串口响应失败，请检查！"));
            if(getTestRunState())
            {
                endTheFlow();
            }
        }
    }
}

/*************************************************************
/函数功能：填充串口发送列表
/函数参数：同命令发送
/函数返回：无
*************************************************************/
void MainWindow::appendTxList(char cmd,char* dat,char len,uint8_t ack)
{
    if(UartConnectStatus())
    {
        uartFrame uartDat;

        uartDat.cmd = cmd;
        for(int i=0;i<len;i++)
        {
            uartDat.dat[i] = dat[i];
        }

        uartDat.len = len;
        uartDat.ack = ack;
        txList.append(uartDat);
    }
}

/*************************************************************
/函数功能：串口字符串后台显示数据传输
/函数参数：revDats:接收字符串数据 dir：传输方向  isHex：是否为hex数据
/函数返回：无
*************************************************************/
void MainWindow::UartByteArraySlot(QByteArray revDats,uartDir dir,bool isHex)
{
    QString strShow;

    if(isHex==Uart_Hex)
        strShow=qtStrDeal->hexToString((unsigned char *)revDats.data(),revDats.length());//hex显示
    else if(isHex==Uart_NHex)
        strShow=revDats;

    if(dir==Uart_Rx)
        ui->textBrowser_mShow->append("Rx: "+strShow);
    else if(dir==Uart_Tx)
        ui->textBrowser_mShow->append("Tx: "+strShow);
    else
        ui->textBrowser_mShow->append("Warn: "+strShow);
}

/*************************************************************
/函数功能：串口接收处理函数
/函数参数：接收帧数据
/函数返回：无
*************************************************************/
void MainWindow::UartRxDealSlot(char cmd,uint8_t dLen,char *dat)
{
    unsigned int tempDat=0;

    if(cmd == CMDWorkCurrent)
    {
        for(int i=0;i<dLen;i++)
        {
            tempDat=((uint8_t)dat[i]<<(i*8))|tempDat;//低位在前，高位在后
        }
        Current = tempDat;

        chartDeal->refreshChart(CHKCurrent,tempDat/1000.0);
    }
}

/*************************************************************
/函数功能：帧响应结果
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::UartRxAckResault(bool result)
{
    //cout << "Frame ack is " << result;
    if(!result)
        cout <<"Frame Chk is Error.";
    else
    {
        ackWait=0;
        actIsRunning = false;//KEY有响应时结束测试流动作
    }
}

/*************************************************************
/函数功能：串口断开
/函数参数：无
/函数返回：无
/备注：原本想做一个按键的翻转，但是只翻转选中项目没有意义，保留功能
*************************************************************/
void MainWindow::UartDisconnect()
{
    //ui->treeWidget->setCheckedState(topUart,false);
}



/*---------------------------------------this is Process option-----------------------------------------*/
/*************************************************************
/函数功能：初始化进程处理
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::initProcessDeal()
{
    PRODeal = new Model_Process;
    connect(PRODeal,SIGNAL(ProcessisOver(uint8_t)),this,SLOT(onProcessOverSlot(uint8_t)));
    connect(PRODeal,SIGNAL(ProcessOutDeal(int,QString)),this,SLOT(onProcessOutputSlot(int,QString)));

    PRODeal->ProcessPathJump(QCoreApplication::applicationDirPath());
    timerProID = startTimer(1);
    isPRORunning=false;
    currentCMDString.clear();
}

/*************************************************************
/函数功能：释放进程处理
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::deleteProcessDeal()
{
    killTimer(timerProID);
    delete PRODeal;
}

/*************************************************************
/函数功能：进程执行处理
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::timerProIDDeal()
{
    if((proList.isEmpty()==false)&&(!proSysIsRunning()))
    {
        //cout << proList.length();
        PRODeal->ProcessStart(PROSYS,proList.first());

        currentCMDString = proList.first();
        isPRORunning=true;
        proList.removeFirst();
    }
}

/*************************************************************
/函数功能：进程输出槽函数处理
/函数参数：进程号  字符串
/函数返回：无
*************************************************************/
void MainWindow::onProcessOutputSlot(int pNum,QString String)
{
    //进程处理
    if((pNum==PROSYS)&&(String.isEmpty()==false))
    {
        //测试进程输出处理
        testProcessOutputDeal(String);

        //处理设备扫描进程
        if((!getTestRunState())&&(currentCMDString == ADBDevs))
        {
            treeWidget_refreshDevNum(String);
        }
        else
        {
            //获取版本信息与报告生成不显示过程:any
            if((testState!=getprop)&&(testState!=report))
                ui->textBrowser_EXEShow->append(String);
        }
        //cout << String;
    }

}

/*************************************************************
/函数功能：进程结束处理
/函数参数：进程号
/函数返回：无
*************************************************************/
void MainWindow::onProcessOverSlot(uint8_t pNum)
{
    if(pNum==PROSYS)
    {
        //cout << currentCMDString << "process is over "<<testState;

        //测试进程结束处理
        testProcessOverDeal();

        //恢复状态
        currentCMDString.clear();
        isPRORunning=false;
    }
}

/*************************************************************
/函数功能：停止系统进程执行logcat
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::proStopSysLogcat()
{
    PRODeal->stopProcess(getDevNumber(),"logcat");
}

/*************************************************************
/函数功能：停止系统进程执行uiautomator
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::proStopSysUiautomator()
{
    PRODeal->stopProcess(getDevNumber(),"uiautomator");
}

/*************************************************************
/函数功能：判断系统进程是否正在执行
/函数参数：无
/函数返回：无
/备注：判断的只是bat中某一条指令是否执行并不能判断bat执行是否正在运行
*************************************************************/
bool MainWindow::proSysIsRunning()
{
    if(PRODeal->GetProcessRunStatus(PROSYS) != noRun)
        return true;
    else
        return false;
}




/*---------------------------------------this is test fun option-----------------------------------------*/
/*************************************************************
/函数功能：点击按键操作
/函数参数：无
/函数返回：无
/备注：手动点击按键，用来测试
*************************************************************/
void MainWindow::on_pushButton_2_clicked()
{
    QStringList theAct=ui->comboBox->currentText().split(':');
    char buf[2]={0};
    if(theAct.isEmpty()==false)
    {
        buf[0] = theAct.at(0).right(1).toInt();//KEY1  取"1"

        if(ui->checkBox->checkState() == Qt::Checked)
            buf[1]=1;
        else
            buf[1]=0;
        appendTxList(CMDClickedKey,buf,2,CMD_NEEDACK);
    }
}

/*************************************************************
/函数功能：初始化按键列表
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::initkeyList()
{
    Model_XMLFile xmlRead;
    QList <keyControl> keyList;
    QStringList comboList;

    xmlRead.readKeyInfoXML(WorkItem,&keyList);

    if(keyList.isEmpty()==false)
    {
        for(int i=0;i<keyList.length();i++)
        {
            if(keyList.at(i).isUse)
            {
                comboList.append("KEY"+QString::number(i+1)+":"+keyList.at(i).name);

                //标记特殊按键处理字符串：
                if(keyList.at(i).type == HardACC)
                {
                    AccKey = comboList.last();
                }
                else if(keyList.at(i).type == HardBAT)
                {
                    BatKey = comboList.last();
                }
            }
        }
    }
    ui->comboBox->clear();
    ui->comboBox->addItems(comboList);
}

/*************************************************************
/函数功能：显示帧或隐藏帧
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::on_pushButtonhide_clicked(bool checked)
{
    if(checked)
    {
        ui->pushButtonhide->setText("显示帧");
        ui->textBrowser_mShow->setVisible(false);
    }
    else
    {
        ui->pushButtonhide->setText("隐藏帧");
        ui->textBrowser_mShow->setVisible(true);
    }
}

/*************************************************************
/函数功能：点击获取参数
/函数参数：无
/函数返回：无
*************************************************************/
void MainWindow::on_btnReadCurrent_clicked()
{
    chkParamFromHardware(0x00);
}



