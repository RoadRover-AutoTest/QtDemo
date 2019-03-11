#ifndef TOOLCONFIG_H
#define TOOLCONFIG_H

#include <QDialog>
#include "Models/model_xmlfile.h"
#include "diareport.h"

namespace Ui {
class toolConfig;
}

class toolConfig : public QDialog
{
    Q_OBJECT

public:
    explicit toolConfig(QWidget *parent = 0);
    ~toolConfig();

private slots:
    void on_btnLocalReport_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_BtnLogcatPath_clicked();

private:
    Ui::toolConfig *ui;

protected:
    void accept();

};

#endif // TOOLCONFIG_H
