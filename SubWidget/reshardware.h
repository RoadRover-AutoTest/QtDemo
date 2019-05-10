#ifndef RESHARDWARE_H
#define RESHARDWARE_H

#include <QDialog>
#include <QCloseEvent>
#include <QInputDialog>
#include <QMenu>
#include <QDesktopServices>
#include <QFileDialog>
#include "Models/model_include.h"
#include "Models/model_inisetting.h"
#include "reshardedit.h"
//#include "resupanddownloads.h"
#include "Models/model_xmlfile.h"
#include "Models/model_uart.h"
#include "Models/model_string.h"
#include "Models/model_delay.h"

namespace Ui {
class ResHardware;
}

class ResHardware : public QDialog
{
    Q_OBJECT

public:
    explicit ResHardware(Model_UART *uart,QWidget *parent = 0);
    ~ResHardware();

private:
    Ui::ResHardware *ui;
    Model_String *strDeal;

    //按键信息
    QList <keyControl> keyList;
    keyControl copyTempKey;//拷贝临时按键

    void fixedKeyEdit();
    void tableWidgetInit();
    void keyisUsetoEnable(int Num,bool isEn);
    void readItemKeyInfo(QString item);

    uint16_t covCANBaudDeal(QString baud);
    void refreshitemName(QString currentText);
    void initkeyList();

    void saveKeysInfoToXML(QString itemName);
    QString g_ItemReadString;

    void uartReadCANParamDeal();
    QString recovCANBaudDeal(uint16_t baud);


    QString g_DisplayBuf[8];//显示缓存
    void DispBufClear();

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

    uint8_t rxCount;


    //bool downIsFileSave();
    //bool downIsUartHardware();

protected:
    void closeEvent(QCloseEvent *event);


private slots:
    void EditKeyClicked();
    void on_cellChanged(int row, int column);
    void customContextMenuUpload_clicked();
    void uploadKeysDeal();
    void customContextMenuDownload_clicked();
    void customContextMenuReset_clicked();
    void itemNameSlot(const QString &arg1);
    void customContextMenuSave_clicked();
    void customContextMenuCopy_clicked();
    void customContextMenuPaste_clicked();
    void customContextMenuDelete_clicked();

    void UartRxDealSlot(char cmd,uint8_t dLen,char *dat);
    void UartRxAckResault(bool ack);
    void UartErrorDeal();
    void UartByteArraySlot(QByteArray revDats,uartDir dir,bool isHex);

    void on_checkBoxENCAN1_clicked(bool checked);
    void on_checkBoxENCAN2_clicked(bool checked);
    void on_horizontalSliderBAT_valueChanged(int value);
    void on_horizontalSliderCCD_valueChanged(int value);
    void on_radioBtn15V_clicked();
    void on_radioBtn24V_clicked();

    void on_pushBtnReadVB_clicked();


    void UpdateScreenSlot(unsigned char line);

    void on_comboBox_MediaTrans_currentIndexChanged(int index);

    void on_pushButton_KeyClicked_clicked();

    void on_tableWidget_customContextMenuRequested(const QPoint &pos);


    void on_pushBtnHelp_clicked();

signals:
    void itemNameChange(QString name);
    void windowClose();
    void keysUploadOver();
};

#endif // RESHARDWARE_H
