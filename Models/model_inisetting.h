#ifndef MODEL_INISETTING_H
#define MODEL_INISETTING_H

#include <QObject>
#include <QSettings>
#include "Models/model_include.h"

class Model_iniSetting
{
public:
    Model_iniSetting();

    QVariant ReadIni_email();
    void WriteIni_email(QVariant value);

    QString ReadIni_item();
    void WriteIni_item(QVariant value);

    int ReadIni_WorkCurrent();
    void WriteIni_WorkCurrent(QVariant value);

    int ReadIni_WorkFrequency();
    void WriteIni_WorkFrequency(QVariant value);

    bool ReadIni_ReportCreat();
    void WriteIni_ReportCreat(QVariant value);




private:
    void WriteIni(QString file,QString Section,QString Key,QVariant value);
    void ReadIni(QString file,QString Section,QString Key,QVariant *value);

};

#endif // MODEL_INISETTING_H
