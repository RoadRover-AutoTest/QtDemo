#include "model_piccompare.h"

Model_PicCompare::Model_PicCompare()
{

}
Model_PicCompare::~Model_PicCompare()
{

}

#if 0
/*************************************************************
/函数功能：比较图片
/函数参数：无
/函数返回：无
*************************************************************/
void Model_PicCompare::Cameracompare(QString comPic,QString fixedPic)
{
    int iDiffNum = CameraPerHash(comPic, fixedPic);

    //得到指纹以后，就可以对比不同的图片，看看64位中有多少位是不一样的。
    //在理论上，这等同于计算"汉明距离"（Hamming distance）。
    //如果不相同的数据位不超过5，就说明两张图片很相似；如果大于10，就说明这是两张不同的图片。
    //if (iDiffNum <= 5)
    //    qDebug()<<"校验结果：非常相似";//ShowList.append("校验结果：非常相似");//QMessageBox::information(this,tr("比较结果"),tr("two images are very similar!"));//+"iDiffNum = "+QString("%1").arg(iDiffNum)
    //else if (iDiffNum > 10)
    //    qDebug()<<"校验结果：完全不同的图片";//ShowList.append("校验结果：完全不同的图片");//QMessageBox::information(this,tr("比较结果"),tr("they are two different images!"));//+"iDiffNum = "+QString("%1").arg(iDiffNum)
    //else
    //    qDebug()<<"校验结果：有些相似";//ShowList.append("校验结果：有些相似");//QMessageBox::information(this,tr("比较结果"),tr("two image are somewhat similar!"));//+"iDiffNum = "+QString("%1").arg(iDiffNum)
}

/*************************************************************
/函数功能：cv：Mat转换为QT：QImage
/函数参数：无
/函数返回：无
*************************************************************/
QImage Model_PicCompare::cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        image.setColorCount(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
        //qDebug() << "CV_8UC4";
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        //qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

/*************************************************************
/函数功能：QT：QImage转换为cv：Mat
/函数参数：无
/函数返回：无
*************************************************************/
cv::Mat Model_PicCompare::QImage2cvMat(QImage image)
{
    cv::Mat mat;
    //qDebug() << image.format();

    if((image.format()==QImage::Format_ARGB32)||(image.format()==QImage::Format_RGB32)
            ||(image.format()==QImage::Format_ARGB32_Premultiplied))
    {
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
    }
    else if(image.format()==QImage::Format_RGB888)
    {
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, CV_BGR2RGB);
    }
    else if(image.format()==QImage::Format_Indexed8)
    {
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
    }

    return mat;
}

/*************************************************************
/函数功能：根据图片所在的路径字符串比较图片
/函数参数：ImageName1,ImageName2图片路径，不要带有汉字：eg:"C:\\Users\\lxj\\Desktop\\Autotest pc\\pic\\img1.jpg"
/函数返回：”汉明距离”(Hamming distance,在信息论中，两个等长字符串之间的汉明距离是两个字符串对应位置的不同字符的个数)。
/       如果不相同的数据位数不超过5，就说明两张图像很相似；如果大于10，就说明这是两张不同的图像。
*************************************************************/
int Model_PicCompare::CameraPerHash(QString ImageName1, QString ImageName2)
{
    cv::Mat matSrc1, matSrc2;

    matSrc1 = cv::imread(ImageName1.toStdString(), CV_LOAD_IMAGE_COLOR);
    matSrc2 = cv::imread(ImageName2.toStdString(), CV_LOAD_IMAGE_COLOR);

    return PerHash(matSrc1,matSrc2);
}

/*************************************************************
/函数功能：直接输入图片进行比较
/函数参数：Image1，Image2：这里为采集的图片和文件中打开的图片对比
/函数返回：”汉明距离”(Hamming distance,在信息论中，两个等长字符串之间的汉明距离是两个字符串对应位置的不同字符的个数)。
/       如果不相同的数据位数不超过5，就说明两张图像很相似；如果大于10，就说明这是两张不同的图像。
*************************************************************/
int Model_PicCompare::CameraPerHash(QImage Image1, QImage Image2)
{
    cv::Mat matSrc1, matSrc2;

    matSrc1 = QImage2cvMat(Image1);
    matSrc2 = QImage2cvMat(Image2);

    return PerHash(matSrc1,matSrc2);
}

/*************************************************************
/函数功能：图片与文件中图片比较
/函数参数：Image1，Image2：这里为采集的图片和文件中打开的图片对比
/函数返回：”汉明距离”(Hamming distance,在信息论中，两个等长字符串之间的汉明距离是两个字符串对应位置的不同字符的个数)。
/       如果不相同的数据位数不超过5，就说明两张图像很相似；如果大于10，就说明这是两张不同的图像。
*************************************************************/
int Model_PicCompare::CameraPerHash(QImage Image1, QString ImageName2)
{
    cv::Mat matSrc1, matSrc2;

    matSrc1 = QImage2cvMat(Image1);
    matSrc2 = cv::imread(ImageName2.toStdString(), CV_LOAD_IMAGE_COLOR);

    return PerHash(matSrc1,matSrc2);
}

/*************************************************************
/函数功能：哈希算法，对比图片相似度
/函数参数：event：定时器事件
/函数返回：无
*************************************************************/
int Model_PicCompare::PerHash(const cv::Mat matSrc1,const cv::Mat matSrc2)//QString ImageName1, QString ImageName2
{
    //1.缩小尺寸
    //将图片缩小到8x8的尺寸，总共64个像素。
    //这一步的作用是去除图片的细节，只保留结构、明暗等基本信息，摒弃不同尺寸、比例带来的图片差异。
    cv::Mat matDst1, matDst2;

    cv::resize(matSrc1, matDst1, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);
    cv::resize(matSrc2, matDst2, cv::Size(8, 8), 0, 0, cv::INTER_CUBIC);//scipy.misc.imresize
    //2.简化色彩
    //将缩小后的图片，转为64级灰度。也就是说，所有像素点总共只有64种颜色。
    cv::cvtColor(matDst1, matDst1, CV_BGR2GRAY);
    cv::cvtColor(matDst2, matDst2, CV_BGR2GRAY);

    //3.计算平均值
    //计算所有64个像素的灰度平均值。
    int iAvg1 = 0, iAvg2 = 0;
    int arr1[64], arr2[64];

    for (int i = 0; i < 8; i++)
    {
        uchar* data1 = matDst1.ptr<uchar>(i);
        uchar* data2 = matDst2.ptr<uchar>(i);

        int tmp = i * 8;

        for (int j = 0; j < 8; j++)
        {
            int tmp1 = tmp + j;

            arr1[tmp1] = data1[j] / 4 * 4;
            arr2[tmp1] = data2[j] / 4 * 4;

            iAvg1 += arr1[tmp1];
            iAvg2 += arr2[tmp1];
        }
    }

    iAvg1 /= 64;
    iAvg2 /= 64;

    //4.比较像素的灰度
    //将每个像素的灰度，与平均值进行比较。大于或等于平均值，记为1；小于平均值，记为0。
    for (int i = 0; i < 64; i++)
    {
        arr1[i] = (arr1[i] >= iAvg1) ? 1 : 0;
        arr2[i] = (arr2[i] >= iAvg2) ? 1 : 0;
    }

    //5.计算哈希值
        //将上一步的比较结果，组合在一起，就构成了一个64位的整数，这就是这张图片的指纹。
    //组合的次序并不重要，只要保证所有图片都采用同样次序就行了。
    int iDiffNum = 0;

    for (int i = 0; i < 64; i++)
    {
        if (arr1[i] != arr2[i])
            ++iDiffNum;
    }
    return iDiffNum;
}

#endif



