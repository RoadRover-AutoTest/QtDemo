#ifndef MODEL_PICCOMPARE_H
#define MODEL_PICCOMPARE_H

#include <QObject>

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

#include "model_include.h"


class Model_PicCompare:public QObject
{
    Q_OBJECT
public:
    Model_PicCompare();
    ~Model_PicCompare();

    void Cameracompare(QString comPic ,QString fixedPic);

private:
    QImage cvMat2QImage(const cv::Mat& mat);
    cv::Mat QImage2cvMat(QImage image);
    int CameraPerHash(QString ImageName1, QString ImageName2);
    int CameraPerHash(QImage Image1, QImage Image2);
    int CameraPerHash(QImage Image1, QString ImageName2);
    int PerHash(const cv::Mat matSrc1,const cv::Mat matSrc2);//QString ImageName1, QString ImageName2
};

#endif // MODEL_PICCOMPARE_H
