#ifndef DEFTHEUNIT_H
#define DEFTHEUNIT_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QFileDialog>
#include "Models/model_include.h"
#include "Models/model_xmlfile.h"
#include "cfglookunit.h"

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

    void editTimeDealSlot();
    void editCheckDealSlot(bool checked);

    void on_tableAction_customContextMenuRequested(const QPoint &pos);



    void on_tableAction_clicked(const QModelIndex &index);

    void on_editActName_editingFinished();

    void on_comboKeyList_activated(const QString &arg1);

    void on_groupKeyONOFF_clicked(bool checked);

    void on_toolButtonSelFile_clicked();

    void on_checkfileMore_clicked(bool checked);

    void on_editFilePath_textChanged(const QString &arg1);

    void on_tableAction_itemChanged(QTableWidgetItem *item);

    void on_actLook_triggered();

    void on_editUnitName_editingFinished();

    void on_spinUnitCycle_editingFinished();

    void on_editUnitDes_destroyed();

private:
    Ui::defTheUnit *ui;
    QList <keyControl> keyList;

    tUnit unitDeal;

    typedef enum
    {

        Col_Name,
        Col_Str

    }actCol_e;

    void inittActionParam(tAction *tact);
    void appendTableAction(tAction act);
    void refreshPropertiesParam(tAction act);
    void refreshKeyList(QString actStr);
    void refreshTimeDeal(tAction act);
    void refreshCheckDeal(QList<checkParam> chkDeal);
    int getTableActionSelRanges();
};

#endif // DEFTHEUNIT_H
