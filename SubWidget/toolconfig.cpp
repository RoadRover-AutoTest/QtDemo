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
    Model_iniSetting email;
    QString emailStr = email.ReadIni_email().toString();
    ui->lineEmail->setText(emailStr);

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
    inicof.WriteIni_item(WorkItem);
    inicof.WriteIni_WorkCurrent(WorkCurrent);
    inicof.WriteIni_WorkFrequency(WorkFrequency);
    inicof.WriteIni_ReportCreat(ReportCreat);

    QDialog::accept();
}




void toolConfig::on_buttonBox_clicked(QAbstractButton *button)
{
    if(ui->buttonBox->button(QDialogButtonBox::Reset)==button)
    {
        ui->comboItemList->setCurrentIndex(0);
        ui->spinCurrent->setValue(700);
        ui->spinFrequency->setValue(1);
        ui->checkReportCreat->setChecked(true);
    }
}
