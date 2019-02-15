#ifndef CFGEDITUNIT_H
#define CFGEDITUNIT_H

#include <QDialog>

#include <QMenu>
#include <QFileDialog>
#include <QTreeWidgetItem>


#include "Models/model_include.h"
#include "Models/model_xmlfile.h"
#include "SubWidget/reshardware.h"
#include "SubWidget/cfglookunit.h"

/*************************************************************
/类：CfgEditUnit
*************************************************************/
namespace Ui {
class CfgEditUnit;
}

class CfgEditUnit : public QDialog
{
    Q_OBJECT

public:
    explicit CfgEditUnit(tUnit *editUnit,QWidget *parent = 0);
    ~CfgEditUnit();

private slots:
    void on_checkBox_timeChanged_clicked(bool checked);

    void on_checkBox_voltChanged_clicked(bool checked);

    void on_treeWidget_listAct_customContextMenuRequested(const QPoint &pos);
    void ActListAddSlot();
    void ActListOpenSlot();

    void on_treeWidget_EditAct_clicked(const QModelIndex &index);

    void on_treeWidget_EditAct_customContextMenuRequested(const QPoint &pos);

    void pushButton_Edit_clicked();
    void comboBox_CurrentChanged(QString string);
    void checkBox_KeyOnOff(bool chicked);

    void on_pushButton_unitLook_clicked();

private:
    Ui::CfgEditUnit *ui;

    tUnit *theUnit;

    void WidgetInit();
    void theUnitWidgetInit();

    QList <keyControl> keyList;
    QString scripePath;
    void EditActList();
    //tType judgetActionType(QTreeWidgetItem * item);

    void EditActAdd(tAction *act);

    tAction eAct;
    void EditActProperties(tAction *act);
    void EditKeyProperties(QString actStr);

    typedef enum
    {
        col=0,          //共1列

        topKey=0,       //顶层按键
        topScript,      //顶层脚本

        keyChild=0,     //按键子树

        scriptChild=0   //脚本子树
    }actList;

    typedef enum
    {
        colAct=0,          //共2列
        colParam,

        topAct=0,       //顶层按键

        childAct=0,     //按键子树

    }actEdit;

    typedef enum
    {
        colPro,
        colValue
    }actProperties;

};

#endif // CFGEDITUNIT_H
