#ifndef MODEL_XMLFILE_H
#define MODEL_XMLFILE_H

#include <QObject>
#include <QDomDocument> //文件
#include <QDomProcessingInstruction> //格式头部
#include <QDomElement>  //元素
#include <QFile>
#include <QTextStream> //文件流

#include "model_include.h"


class Model_XMLFile
{
public:
    Model_XMLFile();

    void createSequenceXML(QString filePath);

    bool hasUnitInfomation(QString filePath,QString unitName);

    void appendSequenceXML(QString filePath,QStringList listUnit ,tAction list);

    void readSequenceXML(QString filePath,QList <tUnit> &tFlow);

    void removeUnitXML(QString filePath,QString unitName);



    void createKeyInfoXML();

    bool hasItemKeyInfomation(QString item);

    void appendKeyInfoXML(QString item,bool isAppend,QStringList list);

    void readKeyInfoXML(QString item, QList <keyControl> *keyList);

    void readKeyItemXML(QStringList *itemList);

private:

    typedef enum
    {
        UnitInfo_n=3,
        KeyInfo_n=8,
    }info_type_e;

    void writeSequenceUnitXML(QDomDocument &doc, QDomElement &root, QStringList &unitList,tAction &list);

    void writeSequenceActXML(QDomDocument &doc, QDomElement &root, tAction &list);



    void writeKeyInfoItemXML(QDomDocument &doc, QDomElement &root, QString &item ,QStringList &list);

    void writeKeyInfoXML(QDomDocument &doc, QDomElement &root, QStringList &list);

    void changedKeyInfoXML(QDomDocument &doc, QDomElement &root, QStringList &list);

};

#endif // MODEL_XMLFILE_H
