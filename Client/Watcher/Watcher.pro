VERSION = 1.0.5
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

QT += core
QT += network
QT -= gui
QT += widgets

CONFIG += c++11

TARGET = Watcher
CONFIG -= app_bundle
CONFIG(release,debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

TEMPLATE = app

SOURCES += main.cpp \
    frame3packet.cpp \
    win32netconfig.cpp \
    anetconfig.cpp \
    controlpacket.cpp \
    settings.cpp

HEADERS += \
    controlpacket.h \
    frame3packet.h \
    anetconfig.h \
    win32netconfig.h \
    settings.h



#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../SDKs/vlcLib/mingw -lvlc -lvlccore -lIphlpapi

#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../SDKs/vlcLib/mingw -lvlc -lvlccore -lIphlpapi


#INCLUDEPATH += $$PWD/../../../../SDKs/vlcLib/include
#DEPENDPATH += $$PWD/../../../../SDKs/vlcLib/include
