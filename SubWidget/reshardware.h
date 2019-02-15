#ifndef RESHARDWARE_H
#define RESHARDWARE_H

#include <QDialog>
#include "Models/model_include.h"
#include "Models/model_inisetting.h"
#include "reshardedit.h"
#include "resupanddownloads.h"

namespace Ui {
class ResHardware;
}

class ResHardware : public QDialog
{
    Q_OBJECT

public:
    explicit ResHardware(QWidget *parent = 0);
    ~ResHardware();

private:
    Ui::ResHardware *ui;

    QList <keyControl> keyList;

    void fixedKeyEdit();
    void tableWidgetInit();
    void keyisUsetoEnable(int Num,bool isEn);
    void readItemKeyInfo(QString item);
    void readItemListInfo(QStringList &itemList);

    typedef enum
    {
        colKey,
        colDes,
        colEdit
    }col;

private slots:
    void EditKeyClicked();
    void on_cellChanged(int row, int column);
    void on_pushButton_inDat_clicked();
    void on_pushButton_outDat_clicked();
    void on_pushButton_reset_clicked();
    void itemNameSlot(const QString &arg1);
};

#endif // RESHARDWARE_H
