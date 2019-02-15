#ifndef MODEL_WINAPI_H
#define MODEL_WINAPI_H

#include <QWidget>

#include <windows.h>
#include <dbt.h>
#include <shlwapi.h>
#include <setupapi.h>
#include <hidsdi.h>

#include "model_include.h"

/*************************************************************
/此处插拔事件只注册USB设备驱动
*************************************************************/
static const GUID GUID_DEVINTERFACE_LIST[] =
{
{ 0xA5DCBF10, 0x6530, 0x11D2, { 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } },  // GUID_DEVINTERFACE_USB_DEVICE
//{ 0x53f56307, 0xb6bf, 0x11d0, { 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b } },  // GUID_DEVINTERFACE_DISK
//{ 0x4D1E55B2, 0xF16F, 0x11CF, { 0x88, 0xCB, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30 } },  // GUID_DEVINTERFACE_HID,
//{ 0xad498944, 0x762f, 0x11d0, { 0x8d, 0xcb, 0x00, 0xc0, 0x4f, 0xc3, 0x35, 0x8c } }   // GUID_NDIS_LAN_CLASS
//{ 0x86e0d1e0, 0x8089, 0x11d0, { 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73 } },// GUID_DEVINTERFACE_COMPORT
//{ 0x4D36E978, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } },// GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR
//{ 0x97F76EF0, 0xF883, 0x11D0, { 0xAF, 0x1F, 0x00, 0x00, 0xF8, 0x00, 0x84, 0x5C } },// GUID_DEVINTERFACE_PARALLEL
//{ 0x811FC6A5, 0xF728, 0x11D0, { 0xA5, 0x37, 0x00, 0x00, 0xF8, 0x75, 0x3E, 0xD1 } } // GUID_DEVINTERFACE_PARCLASS
};

/*************************************************************
/端口设备信息类型结构体
/设备ID
/设备描述
/硬件ID
/服务
/。。。
/备注：同类型的数据信息可参看UsbTreeView.exe软件执行结果
*************************************************************/
typedef struct _USB_DEVICE_PNP_STRINGS
{
    PWSTR DeviceId;//PCHAR
    PWSTR DeviceDesc;//PCHAR
    //PWSTR HwId;//PCHAR
    //PWSTR Service;//PCHAR
    //PWSTR DeviceClass;//PCHAR
    PWSTR PortInfo;//PCHAR
} USB_DEVICE_PNP_STRINGS, *PUSB_DEVICE_PNP_STRINGS;

/*************************************************************
/连接设备结构体信息
*************************************************************/
typedef struct
{
    //USB信息
    QString dNumber;//对子进程来说：设备序列号   对全局进程：执行的命令
    QString Port;
}tDevice_t;



class Model_WinApi:public QWidget//QObject
{
    Q_OBJECT
public:
    Model_WinApi();
    ~Model_WinApi();

    QList<tDevice_t> winAPIGetUSBInfo();

private:
    char FirstDriveFromMask (ULONG unitmask);

    bool GetDeviceProperty(HDEVINFO DeviceInfoSet,PSP_DEVINFO_DATA DeviceInfoData,DWORD Property,LPTSTR *ppBuffer);
    void FreeDeviceProperties(PUSB_DEVICE_PNP_STRINGS *ppDevProps);
    BOOL DriverNameToDeviceInst(tDevice_t *tDevice);

    bool EnumUSBDev(QList<tDevice_t> *devInfo);



protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

signals:
    void USBhDevNotify();

};

#endif // MODEL_WINAPI_H
