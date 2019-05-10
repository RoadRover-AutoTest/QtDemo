#include "model_string.h"


typedef void (Model_String::*TRANS)(unsigned char *buf,unsigned char *len,unsigned char *flag);//函数指针，用来指向编码转换函数
TRANS TransFun;


Model_String::Model_String()
{

}

Model_String::~Model_String()
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


/*****************************************************************************************************************
/函数功能：ASCII，不做任何处理
/函数参数：unsigned char *buf：待转换的数据，最大64byte，同时也是输出;unsigned char *len：字节数;
/函数返回：无
/函数说明：
*****************************************************************************************************************/
void Model_String::ASCIINotTrans(unsigned char *buf,unsigned char *len,unsigned char *flag)
{
    buf = buf;//仅作消掉定义未使用的警告用
    len = len;
    flag = flag;
}

/*****************************************************************************************************************
/函数功能：大端unicode码转换为UTF8
/函数参数：unsigned char *buf：待转换的数据，最大64byte，同时也是输出;unsigned char *len：字节数;
/函数返回：无
/函数说明：传入的unicode每个字符两个字节；ASCII低字节为0x00。传出的UTF8编码ASCII一个字节,汉字三个字节，希伯来字母两个字节
*****************************************************************************************************************/
void Model_String::Uni2UTF8_BigEndian(unsigned char *buf,unsigned char *len,unsigned char *flag)
{
    unsigned char tmp;
    for(int i = 0;i < *len;i += 2)//高低字节交换
    {
        tmp = buf[i];
        buf[i] = buf[i+1];
        buf[i+1] = tmp;
    }
    this->Uni2UTF8_LittleEndian(buf,len,flag);
}

/*****************************************************************************************************************
/函数功能：小端unicode码转换为UTF8
/函数参数：unsigned char *buf：待转换的数据，最大64byte，同时也是输出;unsigned char *len：字节数;
/函数返回：无
/函数说明：传入的unicode每个字符两个字节,ASCII高字节为0x00。传出的UTF8编码ASCII一个字节,汉字三个字节，希伯来字母两个字节
*****************************************************************************************************************/
void Model_String::Uni2UTF8_LittleEndian(unsigned char *buf,unsigned char *len,unsigned char *flag)
{
    unsigned short s;
    char t1,t2,t3;
    unsigned char len2;
    unsigned char tmp[128];//,tmp3[128];
    int j = 0;
    //int k,l,m,n;

    *flag = 0;
    if(*len>64)
        return;
    len2 = *len;

    for(int i = 0; i < *len;i += 2)//unicode每个字符占2byte
    {
        s = ((buf[i]<<8)|buf[i+1]);
        if(s < 0x80)//ASCII 直接去掉高字节0x00,得到的便是UTF8
        {
            tmp[j] = (unsigned char)s;
            j += 1;
        }
        else if((s>=0x80)&&(s<=0x07ff))//此范围对应UTF8两个字节
        {
            *flag = 1;
            t1 = 0xC0;
            t1 |= ((unsigned char)((s>>6)&0x1F));
            t2 = 0x80;
            t2 |= ((unsigned char)(s&0x3F));
            tmp[j++] = t1;
            tmp[j++] = t2;
        }
        else if((s>=0x800)&&(s<=0xffff))//此范围对应UTF8三个字节
        {
            t1 = 0xE0;
            t1 |= ((unsigned char)((s>>12)&0x0f));
            t2 = 0x80;
            t2 |= ((unsigned char)((s>>6)&0x3f));
            t3 = 0x80;
            t3 |= ((unsigned char)(s&0x3f));

            tmp[j++] = t1;
            tmp[j++] = t2;
            tmp[j++] = t3;
            len2 += 1;//长度增加1
        }
    }

    tmp[j] = '\0';//添加结束符
    qDebug("%s",tmp);
    j = 0;
    while(tmp[j] != '\0')
    {
        buf[j] = tmp[j];
        j++;
    }
    for(; j < len2; j++)
    {
        buf[j] = ' ';//不足64byte部分补充空格
    }
    *len = len2;
}

/*************************************************************
/函数功能：媒体数据转换格式
/函数参数：无
/函数返回：无
*************************************************************/
void Model_String::MediaTransModel(uint8_t trans)
{
    switch (trans) {
    case 0x01:
        TransFun = &Uni2UTF8_LittleEndian;
        break;
    case 0x02:
        TransFun = &Uni2UTF8_BigEndian;
    default:
        TransFun = &ASCIINotTrans;
        break;
    }
}


/*************************************************************
/函数功能：CMD_MEDIA_INFO的解析
/函数参数：无
/函数返回：无
*************************************************************/
void Model_String::MediaInfoAnalyze(uint8_t dLen,char *Mediadat,QString *g_DisplayBuf)
{
     unsigned char flag1,flag2,flag3;
     unsigned char g_SmallScreenLineNumber;
    //将接收到的信息存入g_media_info_rec
    g_media_info_rec.lengh  = dLen;
    g_media_info_rec.type   = Mediadat[0];
    g_media_info_rec.param1 = Mediadat[1];
    g_media_info_rec.param2 = Mediadat[2];
    g_media_info_rec.text1_len = Mediadat[3];
    if(g_media_info_rec.text1_len > 64)
        g_media_info_rec.text1_len = 64;
    int i;
    for(i = 0; i < 128; i++)//先清空缓存
    {
        g_media_info_rec.text1[i] = ' ';
        g_media_info_rec.text2[i] = ' ';
        g_media_info_rec.text3[i] = ' ';
    }

    for(i = 0;i < g_media_info_rec.text1_len;i++) //保存text1
    {
        g_media_info_rec.text1[i] = Mediadat[4+i];
    }

    g_media_info_rec.text2_len = Mediadat[4 + g_media_info_rec.text1_len];//text2_len
    if(g_media_info_rec.text2_len > 64)
        g_media_info_rec.text2_len = 64;

    for(i = 0;i < g_media_info_rec.text2_len;i++) //保存text2
    {
        g_media_info_rec.text2[i] = Mediadat[5 + g_media_info_rec.text1_len + i];
    }

    g_media_info_rec.text3_len = Mediadat[5 + g_media_info_rec.text1_len + g_media_info_rec.text2_len];//text3_len
    if(g_media_info_rec.text3_len > 64)
        g_media_info_rec.text3_len = 64;

    for(i = 0;i < g_media_info_rec.text3_len; i++)    //保存text3
    {
        g_media_info_rec.text3[i] = Mediadat[6 + g_media_info_rec.text1_len + g_media_info_rec.text2_len + i];
    }

    //因为Trans之后3个len可能会变化，所以这个判断要放在Trans之前
    if(g_media_info_rec.lengh > (6 + g_media_info_rec.text1_len + g_media_info_rec.text2_len + g_media_info_rec.text3_len))//有param3和param4
    {
        g_media_info_rec.param3 = Mediadat[6 + g_media_info_rec.text1_len + g_media_info_rec.text2_len + g_media_info_rec.text3_len];
        g_media_info_rec.param4 = Mediadat[7 + g_media_info_rec.text1_len + g_media_info_rec.text2_len + g_media_info_rec.text3_len];
    }
    else
    {
        g_media_info_rec.param3 = 0;
        g_media_info_rec.param4 = 0;
    }

    //text1 text2 text3编码转换
    (this->*TransFun)(g_media_info_rec.text1,&g_media_info_rec.text1_len,&flag1);
    (this->*TransFun)(g_media_info_rec.text2,&g_media_info_rec.text2_len,&flag2);
    (this->*TransFun)(g_media_info_rec.text3,&g_media_info_rec.text3_len,&flag3);

    //需要根据type进行的一些特殊处理，以及转存给显示缓存-----------------------------------------------------------------------------
    if((g_media_info_rec.type >= 0x00)&&(g_media_info_rec.type <= 0x30))//类型为媒体信息
    {
        //DispBufClear();//any--清空显存
        qDebug("%x",g_media_info_rec.type);
        switch(g_media_info_rec.type)
        {
        case 0x03:g_DisplayBuf[0] = "Bluetooth &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;//这里为了支持更炫的字体，用了HTML，HTML空格为&nbsp;,一个空格占两个英文字符宽度
        case 0x10:g_DisplayBuf[0] = "Media is off";break;
        case 0x11:g_DisplayBuf[0] = "Music &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
        case 0x12:g_DisplayBuf[0] = "Radio &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
        case 0x13:g_DisplayBuf[0] = "CD&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
        case 0x14:g_DisplayBuf[0] = "DVD &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
        case 0x15:g_DisplayBuf[0] = "Movie &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
        case 0x16:g_DisplayBuf[0] = "A2DP&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
        case 0x17:g_DisplayBuf[0] = "iPod&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
        case 0x18:g_DisplayBuf[0] = "AUX &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
        case 0x19:g_DisplayBuf[0] = "TV&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
        case 0x1A:g_DisplayBuf[0] = "AV&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
        case 0x1B:g_DisplayBuf[0] = "DAB+&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
        case 0x1C:g_DisplayBuf[0] = "CD-MP3&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
        case 0x30:g_DisplayBuf[0] = "MEDIA &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
        default:  g_DisplayBuf[0] = " ";
        }

        unsigned short t1,t2;
        g_SmallScreenLineNumber = 4;

        if(g_media_info_rec.type == 0x12)//类型为Radio收音机
        {
            t1 = g_media_info_rec.param1;
            t2 = g_media_info_rec.param2;
            if((t1 != 0)||(t2 != 0))
            {
                g_DisplayBuf[0] += QString::number(t1);
                g_DisplayBuf[0] += "/";
                g_DisplayBuf[0] += QString::number(t2);
            }
        }
        else    //类型为其他媒体
        {
            t1 = (g_media_info_rec.param1<<8)|g_media_info_rec.param2;
            t2 = (g_media_info_rec.param3<<8)|g_media_info_rec.param4;
            if((t1 != 0)||(t2 != 0))
            {
                g_DisplayBuf[0] += QString::number(t1);
                if(t2 > 0)//总曲目不为0才显示
                {
                    g_DisplayBuf[0] += "/";
                    g_DisplayBuf[0] += QString::number(t2);
                }
            }
        }
        if(g_media_info_rec.type == 0x03)//类型为通话
        {
            if(g_media_info_rec.param1 == 0x01)
                g_DisplayBuf[0] = "来电...";
            else if(g_media_info_rec.param1 == 0x03)
                g_DisplayBuf[0]="正在通话...";
        }
    }
    else
        g_DisplayBuf[0] = " ";

    g_DisplayBuf[1] = " ";//这一行显示空
    g_DisplayBuf[2] = QApplication::translate("Dialog",(char*)g_media_info_rec.text1, 0);//UTF8编码需调用此方法转换才能正常显示
    g_DisplayBuf[3] = QApplication::translate("Dialog",(char*)g_media_info_rec.text2, 0);//UTF8编码需调用此方法转换才能正常显示
    g_DisplayBuf[4] = QApplication::translate("Dialog",(char*)g_media_info_rec.text3, 0);//UTF8编码需调用此方法转换才能正常显示

    //---------------------------------------------------------------------------------------------------------------------
    //发出显示界面更新信号
    UpdateScreenSignal(g_SmallScreenLineNumber);
}

/*************************************************************
/函数功能：CMD_MEDIA_INFO2的解析
/函数参数：无
/函数返回：无
*************************************************************/
void Model_String::MediaInfo2Analyze(uint8_t dLen,char *Mediadat,QString *g_DisplayBuf)
{
    unsigned char flag1,flag2,flag3,flag4,flag5,flag6,flag7;
    unsigned char g_SmallScreenLineNumber;
   //将接收到的信息存入g_media_info2_rec
   g_media_info2_rec.lengh = dLen;
   g_media_info2_rec.text_num = Mediadat[0];
   g_media_info2_rec.type = Mediadat[1];
   g_media_info2_rec.param1 = Mediadat[2];
   g_media_info2_rec.param2 = Mediadat[3];

   int i;
   for(i = 0; i < 128; i++)//先清空缓存
   {
       g_media_info2_rec.text1[i] = ' ';
       g_media_info2_rec.text2[i] = ' ';
       g_media_info2_rec.text3[i] = ' ';
       g_media_info2_rec.text4[i] = ' ';
       g_media_info2_rec.text5[i] = ' ';
       g_media_info2_rec.text6[i] = ' ';
       g_media_info2_rec.text7[i] = ' ';
   }

   if(g_media_info2_rec.text_num >= 1)
   {
       g_SmallScreenLineNumber = 1;
       g_media_info2_rec.text1_len = Mediadat[4];//text1_len
       if(g_media_info2_rec.text1_len > 64)
           g_media_info2_rec.text1_len = 64;
       for(i = 0;i < g_media_info2_rec.text1_len;i++) //保存text1
       {
           g_media_info2_rec.text1[i] = Mediadat[5+i];
       }
   }

   if(g_media_info2_rec.text_num >= 2)
   {
       g_SmallScreenLineNumber = 2;
       g_media_info2_rec.text2_len = Mediadat[5 + g_media_info2_rec.text1_len];//text2_len
       if(g_media_info2_rec.text2_len > 64)
           g_media_info2_rec.text2_len = 64;
       for(i = 0;i < g_media_info2_rec.text2_len;i++) //保存text2
       {
           g_media_info2_rec.text2[i] = Mediadat[6 + g_media_info2_rec.text1_len + i];
       }
   }

   if(g_media_info2_rec.text_num >= 3)
   {
       g_SmallScreenLineNumber = 3;
       g_media_info2_rec.text3_len = Mediadat[6 + g_media_info2_rec.text1_len + g_media_info2_rec.text2_len];//text3_len
       if(g_media_info2_rec.text3_len > 64)
           g_media_info2_rec.text3_len = 64;

       for(i = 0;i < g_media_info2_rec.text3_len; i++)    //保存text3
       {
           g_media_info2_rec.text3[i] = Mediadat[7 + g_media_info2_rec.text1_len + g_media_info2_rec.text2_len + i];
       }
   }

   if(g_media_info2_rec.text_num >= 4)
   {
       g_SmallScreenLineNumber = 4;
       g_media_info2_rec.text4_len = Mediadat[7 + g_media_info2_rec.text1_len + g_media_info2_rec.text2_len + g_media_info2_rec.text3_len];//text4_len
       if(g_media_info2_rec.text4_len > 64)
           g_media_info2_rec.text4_len = 64;

       for(i = 0;i < g_media_info2_rec.text4_len; i++)    //保存text4
       {
           g_media_info2_rec.text4[i] = Mediadat[8 + g_media_info2_rec.text1_len + g_media_info2_rec.text2_len + g_media_info2_rec.text3_len + i];
       }
   }

   if(g_media_info2_rec.text_num >= 5)
   {
       g_SmallScreenLineNumber = 5;
       g_media_info2_rec.text5_len = Mediadat[8 + g_media_info2_rec.text1_len + g_media_info2_rec.text2_len + g_media_info2_rec.text3_len\
                                                + g_media_info2_rec.text4_len];//text5_len
       if(g_media_info2_rec.text5_len > 64)
           g_media_info2_rec.text5_len = 64;

       for(i = 0;i < g_media_info2_rec.text5_len; i++)    //保存text5
       {
           g_media_info2_rec.text5[i] = Mediadat[9 + g_media_info2_rec.text1_len + g_media_info2_rec.text2_len + g_media_info2_rec.text3_len\
                   + g_media_info2_rec.text4_len + i];
       }
   }

   if(g_media_info2_rec.text_num >= 6)
   {
       g_SmallScreenLineNumber = 6;
       g_media_info2_rec.text6_len = Mediadat[9 + g_media_info2_rec.text1_len + g_media_info2_rec.text2_len + g_media_info2_rec.text3_len\
                                                + g_media_info2_rec.text4_len + g_media_info2_rec.text5_len];//text6_len
       if(g_media_info2_rec.text6_len > 64)
           g_media_info2_rec.text6_len = 64;

       for(i = 0;i < g_media_info2_rec.text6_len; i++)    //保存text6
       {
           g_media_info2_rec.text6[i] = Mediadat[10 + g_media_info2_rec.text1_len + g_media_info2_rec.text2_len + g_media_info2_rec.text3_len\
                   + g_media_info2_rec.text4_len + g_media_info2_rec.text5_len + i];
       }
   }

   if(g_media_info2_rec.text_num >= 7)
   {
       g_SmallScreenLineNumber = 7;
       g_media_info2_rec.text7_len = Mediadat[10 + g_media_info2_rec.text1_len + g_media_info2_rec.text2_len + g_media_info2_rec.text3_len\
                                                + g_media_info2_rec.text4_len + g_media_info2_rec.text5_len + g_media_info2_rec.text6_len];//text7_len
       if(g_media_info2_rec.text7_len > 64)
           g_media_info2_rec.text7_len = 64;

       for(i = 0;i < g_media_info2_rec.text7_len; i++)    //保存text7
       {
           g_media_info2_rec.text7[i] = Mediadat[11 + g_media_info2_rec.text1_len + g_media_info2_rec.text2_len + g_media_info2_rec.text3_len\
                   + g_media_info2_rec.text4_len + g_media_info2_rec.text5_len + g_media_info2_rec.text6_len + i];
       }
   }

   //因为Trans之后7个len可能会变化，所以这个判断要放在Trans之前
   if(g_media_info2_rec.lengh > (11 + g_media_info2_rec.text1_len + g_media_info2_rec.text2_len + g_media_info2_rec.text3_len\
                                 + g_media_info2_rec.text4_len + g_media_info2_rec.text5_len + g_media_info2_rec.text6_len\
                                 + g_media_info2_rec.text7_len))//有param3和param4
   {
       g_media_info2_rec.param3 = Mediadat[11 + g_media_info2_rec.text1_len + g_media_info2_rec.text2_len + g_media_info2_rec.text3_len\
                                                + g_media_info2_rec.text4_len + g_media_info2_rec.text5_len + g_media_info2_rec.text6_len\
                                                + g_media_info2_rec.text7_len];
       g_media_info2_rec.param4 = Mediadat[12 + g_media_info2_rec.text1_len + g_media_info2_rec.text2_len + g_media_info2_rec.text3_len\
                                                + g_media_info2_rec.text4_len + g_media_info2_rec.text5_len + g_media_info2_rec.text6_len\
                                                + g_media_info2_rec.text7_len];
   }
   else
   {
       g_media_info2_rec.param3 = 0;
       g_media_info2_rec.param4 = 0;
   }

   //text1 text2 text3 ... 编码转换
   (this->*TransFun)(g_media_info2_rec.text1,&g_media_info2_rec.text1_len,&flag1);
   (this->*TransFun)(g_media_info2_rec.text2,&g_media_info2_rec.text2_len,&flag2);
   (this->*TransFun)(g_media_info2_rec.text3,&g_media_info2_rec.text3_len,&flag3);
   (this->*TransFun)(g_media_info2_rec.text4,&g_media_info2_rec.text4_len,&flag4);
   (this->*TransFun)(g_media_info2_rec.text5,&g_media_info2_rec.text5_len,&flag5);
   (this->*TransFun)(g_media_info2_rec.text6,&g_media_info2_rec.text6_len,&flag6);
   (this->*TransFun)(g_media_info2_rec.text7,&g_media_info2_rec.text7_len,&flag7);

   //需要根据type进行的一些特殊处理，以及转存给显示缓存-----------------------------------------------------------------------------
   if((g_media_info2_rec.type >= 0x03)&&(g_media_info2_rec.type <= 0x30))//类型为媒体信息
   {
       //DispBufClear();//清空显存
       qDebug("media type:%x",g_media_info2_rec.type);
       switch(g_media_info2_rec.type)
       {
       case 0x03:g_DisplayBuf[0] = "Bluetooth &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;//这里为了支持更炫的字体，用了HTML，HTML空格为&nbsp;,一个空格占两个英文字符宽度
       case 0x10:g_DisplayBuf[0] = "Media is off";break;
       case 0x11:g_DisplayBuf[0] = "Music &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
       case 0x12:g_DisplayBuf[0] = "Radio &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
       case 0x13:g_DisplayBuf[0] = "CD&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
       case 0x14:g_DisplayBuf[0] = "DVD &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
       case 0x15:g_DisplayBuf[0] = "Movie &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
       case 0x16:g_DisplayBuf[0] = "A2DP&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
       case 0x17:g_DisplayBuf[0] = "iPod&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
       case 0x18:g_DisplayBuf[0] = "AUX &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
       case 0x19:g_DisplayBuf[0] = "TV&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
       case 0x1A:g_DisplayBuf[0] = "AV&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
       case 0x1B:g_DisplayBuf[0] = "DAB+&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
       case 0x1C:g_DisplayBuf[0] = "CD-MP3&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
       case 0x30:g_DisplayBuf[0] = "MEDIA &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";break;
       default:  g_DisplayBuf[0] = " ";
       }

       unsigned short t1,t2;
       if(g_media_info2_rec.type == 0x12)//类型为Radio收音机
       {
           t1 = g_media_info2_rec.param1;
           t2 = g_media_info2_rec.param2;
           if((t1 != 0)||(t2 != 0))
           {
               g_DisplayBuf[0] += QString::number(t1);
               g_DisplayBuf[0] += "/";
               g_DisplayBuf[0] += QString::number(t2);
           }
       }
       else    //类型为其他媒体
       {
           t1 = (g_media_info2_rec.param1<<8)|g_media_info2_rec.param2;
           t2 = (g_media_info2_rec.param3<<8)|g_media_info2_rec.param4;
           if((t1 != 0)||(t2 != 0))
           {
               g_DisplayBuf[0] += QString::number(t1);
               if(t2 > 0)//总曲目不为0才显示
               {
                   g_DisplayBuf[0] += "/";
                   g_DisplayBuf[0] += QString::number(t2);
               }
           }
       }
       if(g_media_info_rec.type == 0x03)//类型为通话
       {
           if(g_media_info2_rec.param1 == 0x01)
               g_DisplayBuf[0] = "来电...";
           else if(g_media_info2_rec.param1 == 0x03)
               g_DisplayBuf[0]="正在通话...";
       }
   }
   else
       g_DisplayBuf[0] = " ";

   g_DisplayBuf[1] = QApplication::translate("Dialog",(char*)g_media_info2_rec.text1, 0);//UTF8编码需调用此方法转换才能正常显示
   g_DisplayBuf[2] = QApplication::translate("Dialog",(char*)g_media_info2_rec.text2, 0);//UTF8编码需调用此方法转换才能正常显示
   g_DisplayBuf[3] = QApplication::translate("Dialog",(char*)g_media_info2_rec.text3, 0);//UTF8编码需调用此方法转换才能正常显示
   g_DisplayBuf[4] = QApplication::translate("Dialog",(char*)g_media_info2_rec.text4, 0);//UTF8编码需调用此方法转换才能正常显示
   g_DisplayBuf[5] = QApplication::translate("Dialog",(char*)g_media_info2_rec.text5, 0);//UTF8编码需调用此方法转换才能正常显示
   g_DisplayBuf[6] = QApplication::translate("Dialog",(char*)g_media_info2_rec.text6, 0);//UTF8编码需调用此方法转换才能正常显示
   g_DisplayBuf[7] = QApplication::translate("Dialog",(char*)g_media_info2_rec.text7, 0);//UTF8编码需调用此方法转换才能正常显示

   //发出显示界面更新信号
   UpdateScreenSignal(g_SmallScreenLineNumber);
}


