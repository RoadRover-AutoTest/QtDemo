#ifndef DEFTHEUNIT_H
#define DEFTHEUNIT_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QDesktopServices>
#include <QHelpEvent>
#include <QToolTip>
#include "Models/model_include.h"
#include "Models/model_xmlfile.h"
#include "cfglookunit.h"
#include "UnitDeal/testunit.h"

namespace Ui {
class defTheUnit;
}

class defTheUnit : public QMainWindow
{
    Q_OBJECT

public:
    explicit defTheUnit(tUnit *unit,QWidget *parent = 0);
    ~defTheUnit();

private slots:
    void ActionAppendSlot();
    void deleteActionSlot();
    void downActionSlot();
    void upActionSlot();

    void editTimeDealSlot(QString arg1);
    void editCheckDealSlot(bool checked);
    void editErrorDealSlot();

    void unitDeleteSlot();
    void unitClearSlot();

    void on_tableAction_customContextMenuRequested(const QPoint &pos);

    void on_tableAction_clicked(const QModelIndex &index);

    //void on_editActName_textChanged(const QString &arg1);

    void on_comboKeyList_activated(const QString &arg1);

    void on_groupKeyONOFF_clicked(bool checked);

    void on_toolButtonSelFile_clicked();

    void on_checkfileMore_clicked(bool checked);

    void on_editFilePath_textChanged(const QString &arg1);

    void editBatVoltDealSlot(QString volt);

    void editDelayTimeDealSlot(QString time);

    //void on_tableAction_itemChanged(QTableWidgetItem *item);

    void on_actLook_triggered();

    void on_editUnitName_textChanged(const QString &arg1);

    void on_spinUnitCycle_valueChanged(int arg1);

    void on_editUnitDes_textChanged();

    void on_actSave_triggered();

    void on_actApply_triggered();

    void on_actnew_triggered();

    void on_actHelp_triggered();

    void on_listUnit_clicked(const QModelIndex &index);

    void on_listUnit_customContextMenuRequested(const QPoint &pos);

    void on_comboBoxActColInfo_checkedStateChange(int , bool );

private:
    Ui::defTheUnit *ui;
    QList <keyControl> keyList;
    testUnit *unitH;

    tUnit unitDeal;
    QList <tUnit> unitList; //文件中存储的测试单元序列


    typedef enum
    {
        Col_Name,
        Col_Str
    }actCol_e;

    typedef enum
    {
        ACTFront_Interface,
        ACTBack_Interface,
        ACTFront_Picture,
        ACTBack_Picture
    }colInfo_e;

    void inittActionParam(tAction *tact);
    int  tableAction_RowCount();
    void tableAction_Clear();
    void tableAction_Append(tAction act);
    int  tableAction_SelRanges();
    void tableAction_ActReplace(int row,tAction repAct);

    void refreshUnitShow(tUnit unit);
    void refreshPropertiesParam(int index,tAction act);
    void refreshColInfo(QStringList infoDeal);
    void refreshKeyList(QString actStr);
    void refreshTimeDeal(tAction act);
    void refreshCheckDeal(QList<checkParam> chkDeal);
    void refreshErrorDeal(uint8_t errorFlag);

    void moveRow(int nFrom, int nTo );

signals:
    void applyTheUnit(tUnit unit);
};

#endif // DEFTHEUNIT_H
