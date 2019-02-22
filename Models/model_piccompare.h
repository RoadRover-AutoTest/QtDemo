#ifndef MODEL_PICCOMPARE_H
#define MODEL_PICCOMPARE_H

#include <QObject>
//#include "model_include.h"//此处不可添加，将会报错，原因不明

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"
//using namespace cv;

class Model_PicCompare:public QObject
{
    Q_OBJECT
public:
    Model_PicCompare();
    ~Model_PicCompare();

    void Cameracompare(QString pic1 ,QString pic2);

private:
    int PerHash(const cv::Mat matSrc1,const cv::Mat matSrc2);//QString ImageName1, QString ImageName2
};

#endif // MODEL_PICCOMPARE_H
