#ifndef DEFTHEUNIT_H
#define DEFTHEUNIT_H

#include <QMainWindow>
#include "Models/model_include.h"
#include "defaction.h"

namespace Ui {
class defTheUnit;
}

class defTheUnit : public QMainWindow
{
    Q_OBJECT

public:
    explicit defTheUnit(QWidget *parent = 0);
    ~defTheUnit();

private slots:
    void keyActionSlot();
    void scriptActionSlot();

    void on_tableAction_customContextMenuRequested(const QPoint &pos);



    void on_tableAction_clicked(const QModelIndex &index);

    void on_editActName_editingFinished();

private:
    Ui::defTheUnit *ui;

    tUnit unitDeal;

    typedef enum
    {
        Col_Action,
        Col_Name,
        Col_Str,
        Col_WaitTime,
        Col_ChkTime,
        Col_OverTime,
        Col_Check
    }actCol_e;

    void appendTableAction(bool flag,tAction act);
    void initPropertiesParam(bool type,tAction act);
    void getTableActionSelRanges(int *selrow);
};

#endif // DEFTHEUNIT_H
