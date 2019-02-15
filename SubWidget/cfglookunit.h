#ifndef CFGLOOKUNIT_H
#define CFGLOOKUNIT_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "Models/model_include.h"

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

    tUnit *lookUnit;
    QTreeWidgetItem * AddTreeRoot(QString name,QString desc);
};

#endif // CFGLOOKUNIT_H
