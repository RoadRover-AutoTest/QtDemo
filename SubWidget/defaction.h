#ifndef DEFACTION_H
#define DEFACTION_H

#include <QDialog>
#include <QFileDialog>

#include "Models/model_xmlfile.h"

namespace Ui {
class defAction;
}

class defAction : public QDialog
{
    Q_OBJECT

public:
    explicit defAction(bool flag,QWidget *parent = 0);
    ~defAction();

    QString getScriptFilepath();
    QString getKeyString();

private slots:
    void on_toolButton_clicked();

    void on_checkScriptMore_clicked(bool checked);

private:
    Ui::defAction *ui;
    QList <keyControl> keyList;

    bool actflag;//1-Key 0-Script
};

#endif // DEFACTION_H
