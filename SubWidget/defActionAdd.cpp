#include "defActionAdd.h"
#include "ui_defActionAdd.h"

ActionAdd::ActionAdd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActionAdd)
{
    ui->setupUi(this);
}

ActionAdd::~ActionAdd()
{
    delete ui;
}
