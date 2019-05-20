#ifndef CFGLOOKUNIT_H
#define CFGLOOKUNIT_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "Models/model_include.h"
#include "UnitDeal/testunit.h"

namespace Ui {
class CfgLookUnit;
}

class CfgLookUnit : public QDialog
{
    Q_OBJECT

public:
    explicit CfgLookUnit(tUnit *unit, QWidget *parent = 0);
    ~CfgLookUnit();

private:
    Ui::CfgLookUnit *ui;

    QTreeWidgetItem * AddTreeRoot(QString name,QString desc);
};

#endif // CFGLOOKUNIT_H
