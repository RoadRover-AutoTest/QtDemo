#ifndef CFGADDUNIT_H
#define CFGADDUNIT_H

#include <QDialog>

namespace Ui {
class CfgAddUnit;
}

class CfgAddUnit : public QDialog
{
    Q_OBJECT

public:
    explicit CfgAddUnit(QWidget *parent = 0);
    ~CfgAddUnit();

private:
    Ui::CfgAddUnit *ui;
};

#endif // CFGADDUNIT_H
