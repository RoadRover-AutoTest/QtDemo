#include "model_winapi.h"

Model_WinApi::Model_WinApi()
{
    //注册插拔事件
    HDEVNOTIFY hDevNotify;
    DEV_BROADCAST_DEVICEINTERFACE NotifacationFiler;
    ZeroMemory(&NotifacationFiler,sizeof(DEV_BROADCAST_DEVICEINTERFACE));
    NotifacationFiler.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotifacationFiler.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

    for(uint16_t i=0;i<sizeof(GUID_DEVINTERFACE_LIST)/sizeof(GUID);i++)
    {
        NotifacationFiler.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];//GetCurrentUSBGUID();//m_usb->GetDriverGUID();

        hDevNotify = RegisterDeviceNotification((HANDLE)this->winId(),&NotifacationFiler,DEVICE_NOTIFY_WINDOW_HANDLE);
        if(!hDevNotify)
        {
            int Err = GetLastError();
            cout << "注册失败" << Err;
        }
    }


    //枚举当前安卓设备
    //QList<tDevice_t> devInfo;
    //EnumUSBDev(&devInfo);
}

Model_WinApi::~Model_WinApi()
{

}

/*************************************************************
/函数功能：获取windows的事件处理
/函数参数：
/函数返回：无
*************************************************************/
bool Model_WinApi::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(eventType);
    Q_UNUSED(result);

    MSG* msg = reinterpret_cast<MSG*>(message);

    if(msg->message == WM_DEVICECHANGE)
    {
        PDEV_BROADCAST_HDR lpdb = (PDEV_BROADCAST_HDR)msg->lParam;

        if(lpdb->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)//接口设备
        {
            PDEV_BROADCAST_DEVICEINTERFACE pDevInf  = (PDEV_BROADCAST_DEVICEINTERFACE)lpdb;
            QString strname = QString::fromWCharArray(&pDevInf->dbcc_name[4],pDevInf->dbcc_size-116);

            /*是否为安卓车机:过滤掉其余USB设备的插拔信息*/
            if(strname.contains("USB#VID_18D1&PID_D002"))
            {
                //cout << "Event DEVICECHANGE Happend strname:" << strname;

                QStringList tempStr=strname.split("#");
                tDevice_t tDev;
                //tDev.hID=tempStr.at(1);
                tDev.dNumber=tempStr.at(2);

                switch (msg->wParam)
                {
                case DBT_DEVICEARRIVAL:
                    /*插入设备*/

                    //判断序列号字符串是否存在不合格字符 //  获取设备端口号
                    if((tDev.dNumber.contains("&")) || (DriverNameToDeviceInst(&tDev)==false))
                    {
                        cout << "Warn:连接设备失败!";
                    }
                    //cout << "msg->wParam:" << msg->wParam << "插入设备" << tDev.Port <<tDev.dNumber;
                    //ShowList.append(QString("%1%2%3%4%5").arg("msg->wParam： ").arg(msg->wParam).arg("  插入设备  ").arg(tDev.Port).arg(tDev.dNumber));
                    USBhDevNotify();
                break;
                case DBT_DEVICEREMOVECOMPLETE:
                    /*拔出设备*/
                    //cout << "msg->wParam:" << msg->wParam << "移除设备" << tDev.Port <<tDev.dNumber;
                    //ShowList.append(QString("%1%2%3%4%5").arg("msg->wParam：").arg(msg->wParam).arg("  移除设备  ").arg(tDev.Port).arg(tDev.dNumber));
                    USBhDevNotify();
                    break;
                }
            }
        }
    }
    return false;
}

/*------------------------------------------------------------------
   FirstDriveFromMask( unitmask )

   Description
     Finds the first valid drive letter from a mask of drive letters.
     The mask must be in the format bit 0 = A, bit 1 = B, bit 2 = C,
     and so on. A valid drive letter is defined when the
     corresponding bit is set to 1.

   Returns the first drive letter that was found.
--------------------------------------------------------------------*/
char Model_WinApi::FirstDriveFromMask (ULONG unitmask)
{
    char i;

    for (i = 0; i < 26; ++i)
    {
        if (unitmask & 0x1)
            break;
        unitmask = unitmask >> 1;
    }
    return (i + 'A');
}


/*
* 函数名称: EnumUSBDev
* 函数功能: 枚举设备:列出当前连接到PC上的USB设备
* 参    数:
*           devInfo：存放列举的USB设备信息
* 返 回 值: 成功返回TRUE,失败返回FALSE
*/
bool Model_WinApi::EnumUSBDev(QList<tDevice_t> *devInfo)
{
    SP_DEVICE_INTERFACE_DATA            DeviceInterfaceData;            // 保存设备接口信息
    DWORD                               dwRequiredSize = 0;             // 得到详细信息时需要的内存大小
    PSP_INTERFACE_DEVICE_DETAIL_DATA    pDeviceDetailData = NULL;       // 详细信息
    HDEVINFO                            hDeviceInfo;                    // 设备信息集合的指针
    GUID                                InterfaceGUID=GUID_DEVINTERFACE_LIST[0] ;//= g_guidCdrom

    DeviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    //
    // 1.枚举设备
    //
    int i = 0;
    hDeviceInfo = SetupDiGetClassDevs(&InterfaceGUID,NULL,NULL,DIGCF_DEVICEINTERFACE|DIGCF_PRESENT);

    if(hDeviceInfo == INVALID_HANDLE_VALUE)
    {
        cout << "ERROR";
        return FALSE;
    }

    //枚举设备接口:返回结果为0时结束
    while (SetupDiEnumDeviceInterfaces(hDeviceInfo, NULL, &InterfaceGUID, i++, &DeviceInterfaceData))
    {
        // 下面需要得到Interface的详细信息
        // 先得到详细信息需要的内存大小
        if(SetupDiGetDeviceInterfaceDetail(hDeviceInfo, &DeviceInterfaceData, NULL, 0, &dwRequiredSize, NULL))
        {
            cout << "ERROR";
            break;
        }

        if(pDeviceDetailData)
        {
            free(pDeviceDetailData);
            pDeviceDetailData = NULL;
        }
        pDeviceDetailData = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(dwRequiredSize);// 为详细信息分配内存
        if(pDeviceDetailData == NULL)
        {
            cout << "ERROR";
            break;
        }

        // 得到Interface的详细信息
        pDeviceDetailData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
        SP_DEVINFO_DATA DeviceInfoData;
        DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(hDeviceInfo,&DeviceInterfaceData, pDeviceDetailData,dwRequiredSize,&dwRequiredSize,&DeviceInfoData) )
        {
            cout << "ERROR";
            break;
        }
        //cout << QString::fromWCharArray(pDeviceDetailData->DevicePath);

        QStringList devList=QString::fromWCharArray(pDeviceDetailData->DevicePath).toLower().split("#");
        QString hIDStr = devList.at(1);
        tDevice_t tempDevice;

        //只过滤需要的USB接口设备
        if(hIDStr.toUpper() == DEVHID)
        {
            tempDevice.dNumber=devList.at(2);

            //取得设备端口号
            if(DriverNameToDeviceInst(&tempDevice)==false)
            {
                cout <<"ERROR";
            }
            //cout << tempDevice.dNumber << tempDevice.Port;
            devInfo->append(tempDevice);
        }
    }
    if(pDeviceDetailData)
    {
        free(pDeviceDetailData);
        pDeviceDetailData = NULL;
    }
    SetupDiDestroyDeviceInfoList(hDeviceInfo);
    return TRUE;
}



/*****************************************************************************

  FreeDeviceProperties()

  Free the device properties structure

 *****************************************************************************/
void Model_WinApi::FreeDeviceProperties(PUSB_DEVICE_PNP_STRINGS *ppDevProps)
{
    if(ppDevProps == NULL)
    {
        return;
    }

    if(*ppDevProps == NULL)
    {
        return;
    }

 /*   if ((*ppDevProps)->DeviceId != NULL)
    {
        free((*ppDevProps)->DeviceId);
    }

    if ((*ppDevProps)->DeviceDesc != NULL)
    {
        free((*ppDevProps)->DeviceDesc);
    }
*/
    //
    // The following are not necessary, but left in case
    // in the future there is a later failure where these
    // pointer fields would be allocated.
    //

    /*if ((*ppDevProps)->HwId != NULL)
    {
        free((*ppDevProps)->HwId);
    }

    if ((*ppDevProps)->Service != NULL)
    {
        free((*ppDevProps)->Service);
    }

    if ((*ppDevProps)->DeviceClass != NULL)
    {
        free((*ppDevProps)->DeviceClass);
    }*/

  /*  if ((*ppDevProps)->PortInfo != NULL)
    {
        free((*ppDevProps)->PortInfo);
    }
*/
    free(*ppDevProps);
    *ppDevProps = NULL;
}


/*************************************************************
/函数功能：遍历端口信息，匹配端口序列号，若相同取其端口号信息填入
/函数参数：设备参数：序列号+端口号
/函数返回：无
*************************************************************/
BOOL Model_WinApi::DriverNameToDeviceInst(tDevice_t *tDevice)
{
    HDEVINFO         deviceInfo = INVALID_HANDLE_VALUE;
    BOOL             status = TRUE;
    ULONG            deviceIndex;
    SP_DEVINFO_DATA  deviceInfoData;
    GUID             InterfaceGUID=GUID_DEVINTERFACE_LIST[0] ;
    PUSB_DEVICE_PNP_STRINGS DevProps = NULL;

    //通过类型获取设备信息
    deviceInfo = SetupDiGetClassDevs(&InterfaceGUID,NULL,NULL,DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);//DIGCF_ALLCLASSES

    if (deviceInfo == INVALID_HANDLE_VALUE)
    {
        cout<<"ERROR1";
        status = FALSE;
        goto Done;
    }

    deviceIndex = 0;
    deviceInfoData.cbSize = sizeof(deviceInfoData);

    // Allocate device propeties structure
    DevProps = (PUSB_DEVICE_PNP_STRINGS) malloc(sizeof(USB_DEVICE_PNP_STRINGS));

    if(NULL == DevProps)
    {
        cout<<"ERROR1";
        status = FALSE;
        goto Done;
    }

    //枚举设备信息：获取设备信息集合的设备信息元素
    for(deviceIndex = 0; SetupDiEnumDeviceInfo(deviceInfo, deviceIndex, &deviceInfoData); deviceIndex++)
    {
        //获得设备ID
        ULONG len = 0;

        // 获取设备信息：deviceInfo及设备ID的长度len   deviceInfo其中DeviceId非标准ID，该函数获取其ID的长度，再使用下面函数用来获取标准ID
        if ((SetupDiGetDeviceInstanceId(deviceInfo,&deviceInfoData,NULL,0,&len) != FALSE)
                && (GetLastError() != ERROR_INSUFFICIENT_BUFFER))
        {
            status = FALSE;
            goto Done;
        }

        // 获取设备信息：szInstanceId 用来代替原有ID 定义固定大小的数组来存储数据（使用内存分配经常卡死）
        TCHAR szInstanceId[len+1] = {0};
        if (!SetupDiGetDeviceInstanceId(deviceInfo, &deviceInfoData, szInstanceId, len+1, NULL))
        {
            status = FALSE;
            goto Done;
        }

        DevProps->DeviceId = szInstanceId;

        //比较设备ID是否为需要获取的设备ID
        QStringList deviceIdList = QString::fromWCharArray(DevProps->DeviceId).split("\\");
        if((deviceIdList.size() > 2) && (tDevice->dNumber.toUpper() == deviceIdList.at(2)))
        {
            //获得设备本地信息:端口信息（端口号） SPDRP_LOCATION_INFORMATION为获取信息指示，当前为获取本地信息
            GetDeviceProperty(deviceInfo,&deviceInfoData,SPDRP_LOCATION_INFORMATION,&DevProps->PortInfo);

            QString port=QString::fromWCharArray(DevProps->PortInfo);

            tDevice->Port.sprintf("Port%d-%d",port.right(4).toUInt(),port.mid(6,4).toUInt());

            //cout  << QString::fromWCharArray(DevProps->PortInfo) << QString::fromWCharArray(DevProps->DeviceId);
            break;

        }
    }

Done:
    if (status == FALSE)
        cout<<"ERROR";

    if (deviceInfo != INVALID_HANDLE_VALUE)
        SetupDiDestroyDeviceInfoList(deviceInfo);//释放设备信息列表

    if (DevProps != NULL)
        FreeDeviceProperties(&DevProps);//释放内存
    return status;
}



/*************************************************************
/函数功能：得到设备属性信息
/函数参数：Property 需要得到的属性
/函数返回：获取是否成功
/SPDRP_LOCATION_INFORMATION：Location Info本地信息，包含端口号
*************************************************************/
bool Model_WinApi::GetDeviceProperty(HDEVINFO DeviceInfoSet,PSP_DEVINFO_DATA DeviceInfoData,DWORD Property,LPTSTR *ppBuffer)
{
    bool bResult;
    DWORD requiredLength = 0;

    if (ppBuffer == NULL)
    {
        cout<<"ERROR";
        return FALSE;
    }

    *ppBuffer = NULL;

    bResult = SetupDiGetDeviceRegistryProperty(DeviceInfoSet,
                                               DeviceInfoData,
                                               Property ,
                                               NULL,
                                               NULL,
                                               0,
                                               &requiredLength);

    if ((requiredLength == 0) || (bResult != FALSE && (GetLastError() != ERROR_INSUFFICIENT_BUFFER)))
    {
        cout<<"ERROR";
        return FALSE;
    }

    *ppBuffer = (LPTSTR )malloc(requiredLength);

    if (*ppBuffer == NULL)
    {
        cout<<"ERROR";
        return FALSE;
    }

    bResult = SetupDiGetDeviceRegistryProperty(DeviceInfoSet,
                                                DeviceInfoData,
                                                Property ,
                                                NULL,
                                                (PBYTE) *ppBuffer,
                                                requiredLength,
                                                &requiredLength);

    if(bResult == FALSE)
    {
        cout<<"ERROR";
        free(*ppBuffer);
        *ppBuffer = NULL;
        return FALSE;
    }

    return TRUE;
}

/*************************************************************
/函数功能：获取设备连接信息
/函数参数：无
/函数返回：连接列表
*************************************************************/
QList<tDevice_t> Model_WinApi::winAPIGetUSBInfo()
{
    QList<tDevice_t> devInfo;

    EnumUSBDev(&devInfo);

    return devInfo;
}

