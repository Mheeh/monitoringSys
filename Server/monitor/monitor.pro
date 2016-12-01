#-------------------------------------------------
#
# Project created by QtCreator 2016-03-22T15:51:08
#
#-------------------------------------------------
VERSION = 1.5.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT       += core
QT       += gui
QT       += widgets
QT       += network
QT       += multimedia
QT       += multimediawidgets
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = monitor
TEMPLATE = app
CONFIG(release,debug|release):DEFINES += QT_NO_DEBUG_OUTPUT


SOURCES += main.cpp\
        mainwindow.cpp \
    workstation.cpp \
    author.cpp \
    worksetting.cpp \
    anetconfig.cpp \
    controlpacket.cpp \
    settings.cpp \
    win32netconfig.cpp \
    maindialog.cpp \
    clientwatcher.cpp \
    vlcwidgetvideoext.cpp \
    zoomvideodialog.cpp \
    plcdialog.cpp \
    streamsettingsdialog.cpp

HEADERS  += mainwindow.h \
    workstation.h \
    author.h \
    worksetting.h \
    anetconfig.h \
    controlpacket.h \
    settings.h \
    win32netconfig.h \
    maindialog.h \
    clientwatcher.h \
    vlcwidgetvideoext.h \
    zoomvideodialog.h \
    plcdialog.h \
    streamsettingsdialog.h

#LIBS += -LC:/SDKs/vlcLib/mingw -lvlc -lvlccore -lIphlpapi
LIBS += -LC:/SDKs/VLC-Qt_1.1.0_win32_mingw/bin/ -llibVLCQtWidgets -llibVLCQtCore
#INCLUDEPATH += C:/SDKs/vlcLib/include
INCLUDEPATH += C:/SDKs/VLC-Qt_1.1.0_win32_mingw/include
#DEPENDPATH += C:/SDKs/vlcLib/include
DEPENDPATH += C:/SDKs/VLC-Qt_1.1.0_win32_mingw/include

FORMS    += \
    author.ui \
    worksetting.ui \
    clientwatcher.ui \
    mainwindow.ui \
    streamsettingsdialog.ui
