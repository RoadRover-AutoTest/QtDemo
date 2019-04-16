#ifndef MODEL_PICCOMPARE_H
#define MODEL_PICCOMPARE_H

#include <QObject>

//#include <opencv2/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include "opencv/cv.h"
//#include "opencv/cxcore.h"
//#include "opencv/highgui.h"

class Model_PicCompare:public QObject
{
    Q_OBJECT
public:
    Model_PicCompare();
    ~Model_PicCompare();

    bool Cameracompare(QString pic1 ,QString pic2);

private:
    int PerHash(const QString srcpath1,const QString srcpath2 );//QString ImageName1, QString ImageName2
};

#endif // MODEL_PICCOMPARE_H
