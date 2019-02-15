#ifndef DIAREPORT_H
#define DIAREPORT_H

#include <QDialog>
#include <QDir>
#include <QMessageBox>
#include <QDirModel>
#include <QFileDialog>


#include "Models/model_include.h"
#include "Models/model_process.h"

namespace Ui {
class DiaReport;
}

class DiaReport : public QDialog
{
    Q_OBJECT

public:
    explicit DiaReport(QWidget *parent = 0);
    ~DiaReport();

private slots:
    void on_OpenBtn_clicked();

    void on_CreateBtn_clicked();

    void onProcessOverSlot(uint8_t pNum);

    void onProcessOutputSlot(int pNum,QString string);

private:
    Ui::DiaReport *ui;

    Model_Process *proDeal;
    QString resultPath;

    void LoadFiles(QString path, QTreeWidget *treewidget, QTreeWidgetItem *item);
    int isDigitStr(QString src);
};

#endif // DIAREPORT_H
