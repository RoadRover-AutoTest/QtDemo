#ifndef CFGSPARAM_H
#define CFGSPARAM_H

#include <QDialog>
#include <QComboBox>
#include <QFileDialog>
#include "Models/model_include.h"
#include "Models/model_xmlfile.h"

namespace Ui {
class CfgSParam;
}

class CfgSParam : public QDialog
{
    Q_OBJECT

public:
    explicit CfgSParam(QList <tUnit> *thelist,QWidget *parent = 0);
    ~CfgSParam();

private slots:
    void on_toolopenFile_clicked();

private:
    Ui::CfgSParam *ui;

    QList <tUnit> *listUnit;//测试单元列表测试顺序
    QList <keyControl> keyList;

    void initKeyList();
    void initNullChkParam(checkParam *chkParam);


    void initTabACC();
    QString getAccKey();
    void createACCOffAction(tAction *offAction);
    void createACCOnAction(tAction *onAction);
    void createScriptAction(tAction *scriptAction);

    void initTabBAT();
    QString getBatKey();
    void createBatOffAction(tAction *offAction);
    void createBATOnAction(tAction *onAction);
    void createBatScriptAction(tAction *scriptAction);

    void initTabCCD();
    QString getCCDKey();
    void createCCDOffAction(tAction *offAction);
    void createCCDOnAction(tAction *onAction);
    void createCCDScriptAction(tAction *scriptAction);


    void addTheUnit(tUnit *unit,tAction *act);

    typedef enum
    {
        acc_colItem = 1,

        acc_topKey = 0,             //top栏
        acc_topOff,
        acc_topOn,
        acc_topScript,

        acc_Off_WaitTime = 0,       //off1
        acc_Off_Check,

        acc_Off_Change_min=0,         //off2-waittime
        acc_Off_Change_max,
        acc_Off_Change_step,

        acc_Off_Check_time = 0,         //off2-check
        acc_Off_Check_Current,

        acc_On_WaitTime =0,             //on1
        acc_On_EndTime,
        acc_On_CheckCurrent,
        acc_On_CheckMemory,

        acc_On_CurrentValue = 0,        //on2-current
        acc_On_MemoryBool=0,                //on2-memory
        acc_Script_log=0,               //script1

    }acctree_e;

    typedef enum
    {
        bat_colItem = 1,

        bat_topKey=0,
        bat_topOff,
        bat_topOn,
        bat_topScript,

        bat_Off_WaitTime=0,
        bat_Off_ChangeMin=0,
        bat_Off_ChangeMax,
        bat_Off_ChangeStep,

        bat_On_Waittime=0,
        bat_On_EndTime,
        bat_On_CheckCurrent,
        bat_On_CheckMemory,

        bat_On_CurrentValue=0,
        bat_On_MemoryBool=0,
        bat_Script_log=0,
    }battree_e;

    typedef enum
    {
        ccd_colItem = 1,

        ccd_topKey=0,
        ccd_topOn,
        ccd_topOff,
        ccd_topScript,

        ccd_On_Waittime = 0,
        ccd_On_CheckPic,

        ccd_On_ChangeMin=0,
        ccd_On_ChangeMax,
        ccd_On_ChangeStep,

        ccd_Off_WaitTime = 0,
        ccd_Off_CheckFace,
        ccd_Off_CheckSound,

        ccd_Script_log=0,
    }ccdtree_e;

protected:
    void accept();
};

#endif // CFGSPARAM_H
