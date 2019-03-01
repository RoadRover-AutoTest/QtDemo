#ifndef RETABLEWIDGET_H
#define RETABLEWIDGET_H

#include <QObject>
#include <QtGui>
#include <QApplication>
#include <QTableWidget>
#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QSpinBox>

#include "Models/model_include.h"
#include "Models/model_xmlfile.h"
#include "SubWidget/cfgsparam.h"
#include "SubWidget/deftheunit.h"
#include "SubWidget/cfglookunit.h"


//序列列表
enum
{
   seqColName,
   seqColCycle,
   seqColDes
};

class reTableWidget: public QTableWidget
{
    Q_OBJECT
public:
    reTableWidget(QWidget *parent);
    ~reTableWidget();

    void appendTableWidget(QStringList info);
    QString getSequenceFileName();

    QList <tUnit>*unitAll();

protected:
    void mousePressEvent(QMouseEvent *event);

private:
    defTheUnit *defUnit;


    QList <tUnit> theSeqList;
    QString seqSavePath;

    void SaveTheSeq_Deal(QString path);
    void moveRow(int nFrom, int nTo );

    QList <int> selTableRanges();
private slots:
    void deleteSeqfromTableSlot();
    void clearSeqfromTableSlot();
    void RunSeqCustomSlot();
    void RunSeqAllSlot();
    void UpTheUnitSlot();
    void DownTheUnitSlot();
    void onSpbColumnChanged(int value);
    void ScriptCase1Slot();
    void AddTestCasetoListSlot();
    void LookTheUnitSlot();
    void EditTheUnitSlot();

    void applyDefineUnitSlot(tUnit unit);

public slots:
    void onNewSequenceSlot();
    void onOpenSequenceSlot();
    void onSaveSequenceSlot();
    void onSaveAsSequenceSlot();

signals:
    void RunSeqCustom(int row);
    void RunSeqAll();

};

#endif // RETABLEWIDGET_H
