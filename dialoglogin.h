#ifndef DIALOGLOGIN_H
#define DIALOGLOGIN_H

#include <QDialog>
#include <QTranslator>
#include "Models/model_include.h"
#include "Models/model_inisetting.h"

namespace Ui {
class DialogLogin;
}

class DialogLogin : public QDialog
{
    Q_OBJECT

public:
    explicit DialogLogin(QTranslator *translator,QWidget *parent = 0);
    ~DialogLogin();

private:
    Ui::DialogLogin *ui;

    QTranslator *loginTrans;

protected:
    void accept();
private slots:
    void on_comboBoxLanguage_currentIndexChanged(int index);
};

#endif // DIALOGLOGIN_H
