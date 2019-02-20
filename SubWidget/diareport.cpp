#include "diareport.h"
#include "ui_diareport.h"

DiaReport::DiaReport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DiaReport)
{
    ui->setupUi(this);

    ui->textShow->setOpenExternalLinks(true);

    ui->linePath->setText(QCoreApplication::applicationDirPath()+"/result");

    LoadFiles(ui->linePath->text(),ui->treeFile,NULL);

    proDeal = new Model_Process;
    proDeal->Process_Add();//在原有基础上添加进程，报告执行进程为1

    connect(proDeal,SIGNAL(ProcessisOver(uint8_t)),this,SLOT(onProcessOverSlot(uint8_t)));
    connect(proDeal,SIGNAL(ProcessOutDeal(int,QString)),this,SLOT(onProcessOutputSlot(int,QString)));



}

DiaReport::~DiaReport()
{
    delete proDeal;
    delete ui;
}

void DiaReport::LoadFiles(QString path, QTreeWidget *treewidget, QTreeWidgetItem *item)
{
    QDir dir(path);
    if (!dir.exists())
        return;

    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    int size = list.size();
    for (int i = 0; i < size; i++)
    {
        QFileInfo info = list.at(i);
        if (info.fileName() == "." || info.fileName() == "..")
            continue;

        if (info.isDir())
        {
            QTreeWidgetItem *fileItem = new QTreeWidgetItem(QStringList() << info.fileName(), 0);  //0表示目录
            if (treewidget == NULL)
                item->addChild(fileItem);
            else
                treewidget->addTopLevelItem(fileItem);

            if(isDigitStr(info.fileName()) == -1)
                LoadFiles(info.filePath(), NULL, fileItem);
        }
        /*else
        {
            QTreeWidgetItem *fileItem = new QTreeWidgetItem(QStringList() << info.fileName(), 1);   //1表示是文件
            if (treewidget == NULL)
                item->addChild(fileItem);
            else
                treewidget->addTopLevelItem(fileItem);
        }*/
    }
}

/***
  *判断一个字符串是否为纯数字
  */
int DiaReport::isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();//QString 转换为 char*
     const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)
    { //不是纯数字
        return -1;
    }
    else
    { //纯数字
        return 0;
    }
}

void DiaReport::on_OpenBtn_clicked()
{
    QString dirPath=QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("选择结果文件夹"),QDir::currentPath()));

    if(dirPath.isEmpty()==false)
    {
        ui->linePath->setText(dirPath);
        ui->treeFile->clear();
        LoadFiles(ui->linePath->text(),ui->treeFile,NULL);
    }
}

void DiaReport::on_CreateBtn_clicked()
{
    QTreeWidgetItem *curItem = ui->treeFile->currentItem();

    QString path = ui->linePath->text();
    QStringList pathList;
    resultPath.clear();

    while(curItem!=NULL)
    {
        pathList.append(curItem->text(0));
        curItem=curItem->parent();
    }
    for(int i=(pathList.length()-1);i>=0;i--)
    {
        path +="/"+pathList.at(i);
    }

    QString chkPath = path.right(14);
    if(isDigitStr(chkPath) == -1)
    {
        ui->textShow->append("当前选中路径非日期文件，请重新选择...");
        return;
    }
    else
        resultPath = path.mid(path.indexOf("/result/") +8);

    ui->textShow->append(path+"/");

    proDeal->ProcessStart(PROREPORT,PYTHONREPORT(path+"/"));
}

void DiaReport::onProcessOverSlot(uint8_t pNum)
{
    if(pNum == PROREPORT)
    {
        ui->textShow->append(tr("<html><p><a>报告生成结束，请</a><a href=\"%1\">点击查阅</a></p></html>").arg("http://192.168.13.96/result/"+resultPath+"/report.html"));
    }
}
void DiaReport::onProcessOutputSlot(int pNum,QString string)
{
    if(pNum == PROREPORT)
    {
        ui->textShow->append(string);
    }
}
