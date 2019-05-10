#ifndef MODEL_STRING_H
#define MODEL_STRING_H

#include <QObject>
#include <QDebug>
#include <QApplication>


//用来接收CMD_MEDIA_INFO的结构
typedef struct
{
    unsigned char lengh;
    unsigned char type;
    unsigned char param1;
    unsigned char param2;
    unsigned char text1_len;
    unsigned char text1[128];//定义大一点防止越界崩溃
    unsigned char text2_len;
    unsigned char text2[128];
    unsigned char text3_len;
    unsigned char text3[128];
    unsigned char param3;
    unsigned char param4;
}MEDIA_INFO_REC_STR;//媒体信息

//用来接收CMD_MEDIA_INFO2的结构
typedef struct
{
    unsigned char lengh;
    unsigned char text_num;//text个数
    unsigned char type;
    unsigned char param1;
    unsigned char param2;
    unsigned char text1_len;
    unsigned char text1[128];//定义大一点防止越界崩溃
    unsigned char text2_len;
    unsigned char text2[128];
    unsigned char text3_len;
    unsigned char text3[128];
    unsigned char text4_len;
    unsigned char text4[128];
    unsigned char text5_len;
    unsigned char text5[128];
    unsigned char text6_len;
    unsigned char text6[128];
    unsigned char text7_len;
    unsigned char text7[128];
    unsigned char param3;
    unsigned char param4;
}MEDIA_INFO2_REC_STR;//媒体信息

class Model_String:public QObject
{
    Q_OBJECT
public:
    Model_String();
    ~Model_String();

    void StringToHex(QString str, QByteArray &senddata);//字符串转换为16进制，字符串内可有空格，但不可有其余不相干字符

    QString hexToString(unsigned char *in, int len);

    QString StringDeal_Middle(QString strdat, QString start,QString end);

    //小屏显示处理转换字符串:
    void MediaInfoAnalyze(uint8_t dLen,char *Mediadat,QString *g_DisplayBuf);//,TRANS TransFun);
    void MediaInfo2Analyze(uint8_t dLen,char *Mediadat,QString *g_DisplayBuf);//,TRANS TransFun);

    void MediaTransModel(uint8_t trans);//设置媒体转换模式：0x01:小端转换  0x02:大端转换  其他：不转换
    void ASCIINotTrans(unsigned char *buf,unsigned char *len,unsigned char *flag);
    void Uni2UTF8_LittleEndian(unsigned char *buf,unsigned char *len,unsigned char *flag);
    void Uni2UTF8_BigEndian(unsigned char *buf,unsigned char *len,unsigned char *flag);


private:
    char ConvertHexChar(char ch);


    MEDIA_INFO_REC_STR g_media_info_rec;
    MEDIA_INFO2_REC_STR g_media_info2_rec;

signals:
    void UpdateScreenSignal(unsigned char LineNumber);
};

#endif // MODEL_STRING_H
