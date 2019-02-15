#ifndef MODEL_STRING_H
#define MODEL_STRING_H

#include <QObject>
#include <QDebug>

class Model_String
{
public:
    Model_String();

    void StringToHex(QString str, QByteArray &senddata);//字符串转换为16进制，字符串内可有空格，但不可有其余不相干字符

    QString hexToString(unsigned char *in, int len);

private:
    char ConvertHexChar(char ch);
};

#endif // MODEL_STRING_H
