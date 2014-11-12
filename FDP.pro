#-------------------------------------------------
#
# Project created by QtCreator 2014-11-08T18:46:23
#
#-------------------------------------------------

QT += core gui network script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FDP
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    httpdaemon.cpp \
    aesdecoder.cpp \
    fdlogin.cpp \
    fwdownload.cpp \
    downloadtable.cpp

HEADERS  += mainwindow.h \
    httpdaemon.h \
    aesdecoder.h \
    fdlogin.h \
    fwdownload.h \
    downloadtable.h

FORMS    += mainwindow.ui

LIBS += -lmcrypt

