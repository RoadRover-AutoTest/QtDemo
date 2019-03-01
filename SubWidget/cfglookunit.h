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
    explicit CfgLookUnit(tUnit *unit,bool isEdit, QWidget *parent = 0);
    ~CfgLookUnit();

private slots:
    void on_treeWidget_itemChanged(QTreeWidgetItem *item, int column);

private:
    Ui::CfgLookUnit *ui;

    tUnit *lookUnit;
    QTreeWidgetItem * AddTreeRoot(QString name,QString desc);
};

#endif // CFGLOOKUNIT_H
