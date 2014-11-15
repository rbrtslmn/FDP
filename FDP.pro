#-------------------------------------------------
#
# Project created by QtCreator 2014-11-08T18:46:23
#
#-------------------------------------------------

QT += core gui network script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FDP
TEMPLATE = app

SOURCES += \
    main.cpp\
    gui/mainwindow.cpp \
    net/httpdaemon.cpp \
    crypt/aesdecoder.cpp \
    net/fwdownload.cpp \
    model/downloadtable.cpp \
    net/downloadmanager.cpp \
    net/linkgenerator.cpp

HEADERS  += \
    gui/mainwindow.h \
    net/httpdaemon.h \
    crypt/aesdecoder.h \
    net/fwdownload.h \
    model/downloadtable.h \
    net/downloadmanager.h \
    net/linkgenerator.h \
    gui/reloadsettings.h

FORMS    += \
    gui/mainwindow.ui

LIBS += -lmcrypt

