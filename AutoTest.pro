#-------------------------------------------------
#
# Project created by QtCreator 2018-10-23T14:03:01
#
#-------------------------------------------------

QT       += core gui serialport charts xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = AutoTest
TEMPLATE = app

RC_FILE = icon.rc

win32 {

LIBS += -lhid -lsetupapi

}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH +=D:\OpenCV\opencv\BuildMinGW\install\include
#D:\OpenCV\include\opencv\
#            D:\OpenCV\include\opencv2\
#            D:\OpenCV\include
LIBS +=D:\OpenCV\opencv\BuildMinGW\lib\libopencv_*.a\
   #D:\OpenCV\opencv\BuildMinGW\install\x86\mingw\bin\libopencv_core340.dll\
   #D:\OpenCV\opencv\BuildMinGW\install\x86\mingw\bin\libopencv_highgui340.dll\
   #D:\OpenCV\opencv\BuildMinGW\install\x86\mingw\bin\libopencv_imgcodecs340.dll\
   #D:\OpenCV\opencv\BuildMinGW\install\x86\mingw\bin\libopencv_imgproc340.dll\
   #D:\OpenCV\opencv\BuildMinGW\install\x86\mingw\bin\libopencv_features2d340.dll\
   #D:\OpenCV\opencv\BuildMinGW\install\x86\mingw\bin\libopencv_calib3d340.dll
#LIBS+=D:\OpenCV\lib\libopencv_*.a\
        #D:\OpenCV\lib\libopencv_calib3d249.dll.a\
        #D:\OpenCV\lib\libopencv_contrib249.dll.a\
        #D:\OpenCV\lib\libopencv_core249.dll.a\
        #D:\OpenCV\lib\libopencv_features2d249.dll.a\
        #D:\OpenCV\lib\libopencv_flann249.dll.a\
        #D:\OpenCV\lib\libopencv_gpu249.dll.a\
        #D:\OpenCV\lib\libopencv_highgui249.dll.a\
        #D:\OpenCV\lib\libopencv_imgproc249.dll.a\
        #D:\OpenCV\lib\libopencv_legacy249.dll.a\
        #D:\OpenCV\lib\libopencv_ml249.dll.a\
        #D:\OpenCV\lib\libopencv_objdetect249.dll.a\
        #D:\OpenCV\lib\libopencv_video249.dll.a

#语言便签
TRANSLATIONS += uav_tr_laEN.ts uav_tr_laCN.ts


#解决系统不匹配，提示win32问题等//解决办法就是在你的.pro文件中添加已下代码
#win32 {
#QMAKE_LFLAGS_WINDOWS = /SUBSYSTEM:WINDOWS,5.01
#QMAKE_LFLAGS_CONSOLE = /SUBSYSTEM:CONSOLE,5.01

#DEFINES += _ATL_XP_TARGETING
#QMAKE_CFLAGS += /D_USING_V140_SDK71_
#QMAKE_CXXFLAGS += /D_USING_V140_SDK71_
#LIBS += -L$$quote(C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Lib)
#INCLUDEPATH += $$quote(C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\Include)
#}#//完美解决


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    Models/model_string.cpp \
    Models/model_uart.cpp \
    Models/model_delay.cpp \
    Models/model_process.cpp \
    Models/model_inisetting.cpp \
    Models/model_include.cpp \
    Rewrite/retablewidget.cpp \
    Models/model_xmlfile.cpp \
    SubWidget/cfglookunit.cpp \
    SubWidget/reshardware.cpp \
    SubWidget/reshardedit.cpp \
    Rewrite/retreewidget.cpp \
    SubWidget/chartwidget.cpp \
    Models/model_winapi.cpp \
    SubWidget/diareport.cpp \
    SubWidget/toolconfig.cpp \
    Models/model_piccompare.cpp \
    SubWidget/deftheunit.cpp \
    Models/model_threadlog.cpp \
    dialoglogin.cpp \
    Rewrite/recheckcombobox.cpp \
    UnitDeal/model_taction.cpp \
    UnitDeal/model_tflow.cpp \
    UnitDeal/model_tunit.cpp \
    UnitDeal/testunit.cpp

HEADERS += \
        mainwindow.h \
    Models/model_string.h \
    Models/model_uart.h \
    Models/model_delay.h \
    Models/model_process.h \
    Models/model_include.h \
    Models/model_inisetting.h \
    Rewrite/retablewidget.h \
    Models/model_xmlfile.h \
    SubWidget/cfglookunit.h \
    SubWidget/reshardware.h \
    SubWidget/reshardedit.h \
    Rewrite/retreewidget.h \
    SubWidget/chartwidget.h \
    Models/model_winapi.h \
    SubWidget/diareport.h \
    SubWidget/toolconfig.h \
    Models/model_piccompare.h \
    SubWidget/deftheunit.h \
    Models/model_threadlog.h \
    dialoglogin.h \
    Rewrite/recheckcombobox.h \
    UnitDeal/model_taction.h \
    UnitDeal/model_tflow.h \
    UnitDeal/model_tunit.h \
    UnitDeal/testunit.h

FORMS += \
        mainwindow.ui \
    SubWidget/cfglookunit.ui \
    SubWidget/reshardware.ui \
    SubWidget/reshardedit.ui \
    SubWidget/chartwidget.ui \
    SubWidget/diareport.ui \
    SubWidget/toolconfig.ui \
    SubWidget/deftheunit.ui \
    dialoglogin.ui

RESOURCES += \
    Images/images.qrc

DISTFILES += \
    record.txt \
    Backups/tempfile.txt

#未使用变量不予警告
#方法二：在提示警告函数里面添加Q_UNUSED(arg1);
#QMAKE_CXXFLAGS += -Wno-unused-parameter

#QMAKE_CXXFLAGS += -Wno-unused-variable
