#ifndef RESHARDWARE_H
#define RESHARDWARE_H

#include <QDialog>
#include <QCloseEvent>
#include <QInputDialog>
#include "Models/model_include.h"
#include "Models/model_inisetting.h"
#include "reshardedit.h"
//#include "resupanddownloads.h"
#include "Models/model_xmlfile.h"
#include "Models/model_uart.h"
#include "Models/model_string.h"

#define proValueAdd 100/(MaxKey+1)


namespace Ui {
class ResHardware;
}

class ResHardware : public QDialog
{
    Q_OBJECT

public:
    explicit ResHardware(QWidget *parent = 0);
    ~ResHardware();

private:
    Ui::ResHardware *ui;

    QList <keyControl> keyList;

    void fixedKeyEdit();
    void tableWidgetInit();
    void keyisUsetoEnable(int Num,bool isEn);
    void readItemKeyInfo(QString item);
    void readItemListInfo(QStringList &itemList);

    uint16_t covCANBaudDeal(QString baud);
    void usartTXStatusDeal(bool status,uint8_t transType);
void refreshitemName(QString currentText);

    typedef enum
    {
        colKey,
        colDes,
        colEdit
    }col;

    typedef enum
    {
        usart_NONE,
        usart_DownKeyInfo,
        usart_UpKeyInfo,
        usart_SignalInfo,
    }transType_e;

    Model_UART *keyUart;

    bool isStartUartTx;

    bool isDownLoadLast;
    uint8_t gcv_transType;

    bool isAck;

    int timeIDSendUart;

    uint8_t rxCount;

    uint8_t downloadIndex;

    uint8_t txCount;

    void downUartDeal();

    void upUartDeal();

    //bool downIsFileSave();
    //bool downIsUartHardware();

protected:
    void timerEvent(QTimerEvent *event);
    void closeEvent(QCloseEvent *event);


private slots:
    void EditKeyClicked();
    void on_cellChanged(int row, int column);
    void on_pushButton_inDat_clicked();
    void on_pushButton_outDat_clicked();
    void on_pushButton_reset_clicked();
    void itemNameSlot(const QString &arg1);
    void on_pushButtonSave_clicked();
    void on_checkBoxENUart_clicked(bool checked);

    void UartRxDealSlot(char cmd,uint8_t dLen,char *dat);
    void UartRxAckResault(bool ack);

    void on_checkBoxENCAN1_clicked(bool checked);
    void on_checkBoxENCAN2_clicked(bool checked);
    void on_horizontalSliderBAT_valueChanged(int value);
    void on_horizontalSliderCCD_valueChanged(int value);
    void on_radioBtn15V_clicked();
    void on_radioBtn24V_clicked();

signals:
    void itemNameChange(QString name);
};

#endif // RESHARDWARE_H
