#include "model_string.h"

Model_String::Model_String()
{

}



/*************************************************************
/函数功能：字符串转换为16进制，字符串内可有空格，但不可有其余不相干字符
/函数参数：str:数据字符串，senddata:16进制数据字符数组
/函数返回：无
备注：ERROR：奇数个数据转换后有误
*************************************************************/
void Model_String::StringToHex(QString str, QByteArray &senddata)
{
    int hexdata,lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len/2);
    char lstr,hstr;

    for(int i=0; i<len; )
    {
        hstr=str[i].toLatin1();//.toAscii();

        if(hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if(i > len)
            break;
        else if(i==len)
            lstr = '0';//处理奇数个数据时，最后一个位补0，（默认输出为f）
        else
            lstr = str[i].toLatin1();//.toAscii();

        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata*16+lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}


/*************************************************************
/函数功能：数据字符转换为16进制字符
/函数参数：ch:数据字符
/函数返回：char:转换后是16进制数据
*************************************************************/
char Model_String::ConvertHexChar(char ch)
{
    if((ch >= '0') && (ch <= '9'))
            return ch-0x30;
        else if((ch >= 'A') && (ch <= 'F'))
            return ch-'A'+10;
        else if((ch >= 'a') && (ch <= 'f'))
            return ch-'a'+10;
        else return (-1);
}

/*
 * 16进制数转换成QString显示
 */
QString Model_String::hexToString(unsigned char *in, int len)
{
    int i;
    unsigned char inChar,hi,lo;
    QString s;

    for (i=0; i<len; i++)
    {
        inChar = in[i];

        hi = (inChar & 0xF0) >> 4;
        if (hi > 9)
            hi = 'A' + (hi - 0x0A);
        else
            hi += 0x30;
        s.append(hi);

        lo = inChar & 0x0F;
        if (lo > 9)
            lo = 'A' + (lo - 0x0A);
        else
            lo += 0x30;
        s.append(lo);

        s.append(0x20);
    }

    return s;
}


/*
 *描述：在一个字符串中取以start字符串开始以end结束的字符串数据
 *参数：strdat：源字符串，start：开始字符串，end：结束字符串
 *返回值：目标字符串
*/
QString Model_String::StringDeal_Middle(QString strdat, QString start,QString end)
{
    int cursor_l,cursor_r;//光标位置
    int len ;

    cursor_l=strdat.indexOf(start);
    cursor_r=strdat.indexOf(end);

    if(cursor_r<cursor_l)
    {
        cursor_r=cursor_l+strdat.mid(cursor_l).indexOf(end);//必须为前一个字节的后面字节
    }
    if((cursor_l>0)&&(cursor_r>0))
    {
        len=cursor_r-cursor_l;
        return strdat.mid(cursor_l,len);
    }
    return "null";
}

