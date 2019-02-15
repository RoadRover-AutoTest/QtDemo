#include "model_delay.h"

Model_Delay::Model_Delay()
{

}



/*************************************************************
/函数功能：ms延时函数
/函数参数：msec：时间
/函数返回：无
*************************************************************/
void Model_Delay::usleep(unsigned int msec)
{
#if 0  /* 方法1 */
    QElapsedTimer t;

    t.start();
    while (t.elapsed() < msec) {
        QCoreApplication::processEvents();
    }
#endif

#if 1  /* 方法2 */
    QTime dieTime = QTime::currentTime().addMSecs(msec);

    while (QTime::currentTime() < dieTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
#endif
}


