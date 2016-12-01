QT += core
QT -= gui

CONFIG += c++11

TARGET = starter
#CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    runner.cpp

HEADERS += \
    runner.h
