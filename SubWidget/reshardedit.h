#ifndef RESHARDEDIT_H
#define RESHARDEDIT_H

#include <QDialog>
#include "Models/model_include.h"


namespace Ui {
class ResHardEdit;
}

class ResHardEdit : public QDialog
{
    Q_OBJECT

public:
    explicit ResHardEdit(keyControl *keyInfo,QWidget *parent = 0);
    ~ResHardEdit();

    keyControl *getCurrentKeyInfo();

private slots:
    void on_comboBox_currentIndexChanged(int index);

    void on_lineEdit_editingFinished();

    void on_CAN1Changed();

    void on_CAN2Changed();

    void on_KeyTypeChanged();

private:
    Ui::ResHardEdit *ui;

    keyControl *editKeyInfo;

    void showlabel();
    void keyEditInit();
};

#endif // RESHARDEDIT_H
