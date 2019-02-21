#include "defaction.h"
#include "ui_defaction.h"

defAction::defAction(bool flag,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::defAction)
{
    ui->setupUi(this);

    actflag = flag;
    ui->stackedWidget->setCurrentIndex(actflag);

    //KEY
    if(actflag)
    {
        Model_XMLFile xmlRead;

        xmlRead.readKeyInfoXML(WorkItem,&keyList);

        for(int i=0;i<keyList.length();i++)
        {
            if(keyList.at(i).isUse)
                ui->comboKeyList->addItem("KEY"+QString::number(i+1)+":"+keyList.at(i).name);
        }

    }
    //Script
    else
    {

    }
}

defAction::~defAction()
{
    delete ui;
}

void defAction::on_toolButton_clicked()
{
    QString dirPath;
    if(ui->checkScriptMore->isChecked())
    {
        //选择脚本文件夹
        dirPath=QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("选择脚本文件"),QDir::currentPath()));
    }
    else
    {
        //选择脚本文件
        dirPath=QFileDialog::getOpenFileName(this , tr("选择脚本文件") , "" , tr("Text Files(*.bat)"));
    }
    if(!dirPath.isEmpty())
        ui->editFilePath->setText(dirPath);
}

void defAction::on_checkScriptMore_clicked(bool checked)
{
    if(checked)
        ui->labelfile->setText("文件夹：");
    else
        ui->labelfile->setText("文件：");
}

QString defAction::getScriptFilepath()
{
    return ui->editFilePath->text();
}

QString defAction::getKeyString()
{
    QString curKey = ui->comboKeyList->currentText();
    for(int i=0;i<keyList.length();i++)
    {
        keyControl keyDat = keyList.at(i);
        QString keyShow = "KEY"+QString::number(i+1)+":"+keyDat.name;
        if(curKey==keyShow)
        {
            if(keyDat.type!=Can1_1)
            {
                if(ui->radioKeyON->isChecked())
                    curKey+=":on";
                else
                    curKey+=":off";
            }
            break;
        }
    }
    return curKey;
}
