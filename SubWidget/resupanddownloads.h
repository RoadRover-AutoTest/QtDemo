#ifndef RESUPANDDOWNLOADS_H
#define RESUPANDDOWNLOADS_H

#include <QDialog>
#include "Models/model_include.h"
#include "Models/model_xmlfile.h"
#include "Models/model_uart.h"
#include "Models/model_string.h"

#define proValueAdd 100/(MaxKey+2)



namespace Ui {
class ResUpAndDownLoads;
}

class ResUpAndDownLoads : public QDialog
{
    Q_OBJECT

public:
    explicit ResUpAndDownLoads(bool dir,QList <keyControl> *keyInfo,QWidget *parent = 0);
    ~ResUpAndDownLoads();

    QString getCurrentItem();

    void setdownItemName(QString name);

private slots:
    void UartRxDealSlot(char cmd,uint8_t dLen,char *dat);
    void UartRxAckResault(bool ack);
    void on_pushButton_OUT_clicked();

    void on_pushButton_IN_clicked();

    void on_pushButton_UART_clicked(bool checked);

private:
    Ui::ResUpAndDownLoads *ui;

    QList <keyControl> *keyInfoList;

    Model_UART *keyUart;

    bool isStartUartTx;

    bool isDown;

    bool isAck;

    int timeIDSendUart;

    uint8_t rxCount;

    uint8_t keyIndex;

    uint8_t txCount;

    void downUartDeal();

    void upUartDeal();

    bool downIsFileSave();
    bool downIsUartHardware();

protected:
    void timerEvent(QTimerEvent *event);
};

#endif // RESUPANDDOWNLOADS_H
