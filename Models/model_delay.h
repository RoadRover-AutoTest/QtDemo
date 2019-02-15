#ifndef MODEL_DELAY_H
#define MODEL_DELAY_H

#include <QObject>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>

class Model_Delay
{
public:
    Model_Delay();


    void usleep(unsigned int msec);
};

#endif // MODEL_DELAY_H
