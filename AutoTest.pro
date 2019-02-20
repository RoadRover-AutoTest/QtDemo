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


INCLUDEPATH +=D:\OpenCV\include\opencv\
            D:\OpenCV\include\opencv2\
            D:\OpenCV\include

LIBS+=D:\OpenCV\lib\libopencv_*.a\
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



SOURCES += \
        main.cpp \
        mainwindow.cpp \
    Models/model_string.cpp \
    Models/model_uart.cpp \
    Models/model_delay.cpp \
    Models/model_tflow.cpp \
    Models/model_process.cpp \
    Models/model_inisetting.cpp \
    SubWidget/cfgsparam.cpp \
    Models/model_include.cpp \
    Rewrite/retablewidget.cpp \
    Models/model_xmlfile.cpp \
    SubWidget/cfglookunit.cpp \
    SubWidget/reshardware.cpp \
    SubWidget/reshardedit.cpp \
    SubWidget/resupanddownloads.cpp \
    Rewrite/retreewidget.cpp \
    SubWidget/cfgeditunit.cpp \
    SubWidget/cfgaddunit.cpp \
    SubWidget/chartwidget.cpp \
    Models/model_winapi.cpp \
    Models/model_tunit.cpp \
    Models/model_taction.cpp \
    SubWidget/diareport.cpp \
    SubWidget/toolconfig.cpp \
    Models/model_piccompare.cpp

HEADERS += \
        mainwindow.h \
    Models/model_string.h \
    Models/model_uart.h \
    Models/model_delay.h \
    Models/model_tflow.h \
    Models/model_process.h \
    Models/model_include.h \
    Models/model_inisetting.h \
    SubWidget/cfgsparam.h \
    Rewrite/retablewidget.h \
    Models/model_xmlfile.h \
    SubWidget/cfglookunit.h \
    SubWidget/reshardware.h \
    SubWidget/reshardedit.h \
    SubWidget/resupanddownloads.h \
    Rewrite/retreewidget.h \
    SubWidget/cfgeditunit.h \
    SubWidget/cfgaddunit.h \
    SubWidget/chartwidget.h \
    Models/model_winapi.h \
    Models/model_tunit.h \
    Models/model_taction.h \
    SubWidget/diareport.h \
    SubWidget/toolconfig.h \
    Models/model_piccompare.h

FORMS += \
        mainwindow.ui \
    SubWidget/cfgsparam.ui \
    SubWidget/cfglookunit.ui \
    SubWidget/reshardware.ui \
    SubWidget/reshardedit.ui \
    SubWidget/resupanddownloads.ui \
    SubWidget/cfgeditunit.ui \
    SubWidget/cfgaddunit.ui \
    SubWidget/chartwidget.ui \
    SubWidget/diareport.ui \
    SubWidget/toolconfig.ui

RESOURCES += \
    Images/images.qrc

DISTFILES += \
    history.txt \
    record.txt \
    Backups/tempfile.txt

#未使用变量不予警告
#方法二：在提示警告函数里面添加Q_UNUSED(arg1);
#QMAKE_CXXFLAGS += -Wno-unused-parameter

#QMAKE_CXXFLAGS += -Wno-unused-variable
