#include "cfgaddunit.h"
#include "ui_cfgaddunit.h"

CfgAddUnit::CfgAddUnit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CfgAddUnit)
{
    ui->setupUi(this);
}

CfgAddUnit::~CfgAddUnit()
{
    delete ui;
}
