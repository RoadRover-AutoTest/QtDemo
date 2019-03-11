#include "toolconfig.h"
#include "ui_toolconfig.h"

#include "Models/model_inisetting.h"
#include "Models/model_include.h"


toolConfig::toolConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::toolConfig)
{
    ui->setupUi(this);

    //显示项目名：
    QStringList itemList;
    Model_XMLFile xmlRead;
    xmlRead.readKeyItemXML(&itemList);
    ui->comboItemList->addItems(itemList);
    ui->comboItemList->setCurrentText(WorkItem);

    //修改参数：
    ui->spinCurrent->setValue(WorkCurrent);
    ui->spinFrequency->setValue(WorkFrequency);
    ui->checkReportCreat->setChecked(ReportCreat);

    //显示邮箱：
    Model_iniSetting iniDeal;
    QString emailStr = iniDeal.ReadIni_email().toString();
    ui->lineEmail->setText(emailStr);

    ui->checkLogcat->setChecked(iniDeal.ReadIni_item("LogcatEnable").toBool());
    ui->lineLogcatPath->setText(iniDeal.ReadIni_item("LogcatPath").toString());

}

toolConfig::~toolConfig()
{
    delete ui;
}


/*************************************************************
/函数功能：生成报告
/函数参数：无
/函数返回：无
*************************************************************/
void toolConfig::on_btnLocalReport_clicked()
{
    DiaReport *reportDeal = new DiaReport;

    reportDeal->exec();
    delete reportDeal;
}

/*************************************************************
/函数功能：accept确认参数
/函数参数：无
/函数返回：无
*************************************************************/
void toolConfig::accept()
{
    //修改参数：
    WorkItem = ui->comboItemList->currentText();
    WorkCurrent = ui->spinCurrent->value();
    WorkFrequency = ui->spinFrequency->value();
    ReportCreat = ui->checkReportCreat->isChecked();

    //保存数据：
    Model_iniSetting inicof;
    inicof.WriteIni_email(ui->lineEmail->text());
    inicof.WriteIni_item("item_Name",WorkItem);
    inicof.WriteIni_item("WorkCurrent",WorkCurrent);
    inicof.WriteIni_item("WorkFrequency",WorkFrequency);
    inicof.WriteIni_item("ReportCreat",ReportCreat);

    inicof.WriteIni_item("LogcatEnable",ui->checkLogcat->isChecked());
    inicof.WriteIni_item("LogcatPath",ui->lineLogcatPath->text());


    QDialog::accept();
}

/*************************************************************
/函数功能：重置参数
/函数参数：无
/函数返回：无
*************************************************************/
void toolConfig::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->button(QDialogButtonBox::Reset)==button)
    {
        ui->comboItemList->setCurrentIndex(0);
        ui->spinCurrent->setValue(700);
        ui->spinFrequency->setValue(1);
        ui->checkReportCreat->setChecked(true);
        ui->checkLogcat->setChecked(false);
        ui->lineLogcatPath->setText("");
    }
}

/*************************************************************
/函数功能：Logcat路径选择
/函数参数：无
/函数返回：无
*************************************************************/
void toolConfig::on_BtnLogcatPath_clicked()
{
    QString dirPath=QFileDialog::getOpenFileName(this , tr("Logcat文件") , "" , tr("Text Files(*.bat)"));

    if(dirPath.isEmpty()==false)
        ui->lineLogcatPath->setText(dirPath);
}
