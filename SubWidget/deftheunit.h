#ifndef DEFTHEUNIT_H
#define DEFTHEUNIT_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QFileDialog>
#include <QDesktopServices>
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
    explicit defTheUnit(tUnit *unit,QWidget *parent = 0);
    ~defTheUnit();

private slots:
    void ACCONActionSlot();
    void ACCOFFActionSlot();
    void BATONActionSlot();
    void BATOFFActionSlot();
    void CCDONActionSlot();
    void CCDOFFActionSlot();
    void keyActionSlot();
    void scriptActionSlot();
    void deleteActionSlot();
    void downActionSlot();
    void upActionSlot();

    void editTimeDealSlot(QString arg1);
    void editCheckDealSlot(bool checked);

    void on_tableAction_customContextMenuRequested(const QPoint &pos);

    void on_tableAction_clicked(const QModelIndex &index);

    void on_editActName_textChanged(const QString &arg1);

    void on_comboKeyList_activated(const QString &arg1);

    void on_groupKeyONOFF_clicked(bool checked);

    void on_toolButtonSelFile_clicked();

    void on_checkfileMore_clicked(bool checked);

    void on_editFilePath_textChanged(const QString &arg1);

    void on_tableAction_itemChanged(QTableWidgetItem *item);

    void on_actLook_triggered();

    void on_editUnitName_textChanged(const QString &arg1);

    void on_spinUnitCycle_valueChanged(int arg1);

    void on_editUnitDes_textChanged();

    void on_actSave_triggered();

    void on_actApply_triggered();

    void on_comboUnitList_activated(int index);

    void on_actnew_triggered();

    void on_actdel_triggered();

    void on_actclear_triggered();

    void on_actHelp_triggered();

private:
    Ui::defTheUnit *ui;
    QList <keyControl> keyList;

    tUnit unitDeal;
    QList <tUnit> unitList; //文件中存储的测试单元序列


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
    void changedInfoFlagDeal();
    int getTableActionSelRanges();

    void moveRow(int nFrom, int nTo );

signals:
    void applyTheUnit(tUnit unit);

};

#endif // DEFTHEUNIT_H
