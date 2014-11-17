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
    model/downloadtable.cpp \
    crypt/aesdecoder.cpp \
    net/httpdaemon.cpp \
    net/linkgenerator.cpp \
    net/fwdownload.cpp \
    net/downloadmanager.cpp

HEADERS  += \
    gui/mainwindow.h \
    gui/reloadsettings.h \
    model/downloadtable.h \
    crypt/aesdecoder.h \
    net/httpdaemon.h \
    net/linkgenerator.h \
    net/fwdownload.h \
    net/downloadmanager.h \
    net/downloadstatus.h \
    net/downloadinformation.h \
    net/logindata.h

FORMS    += \
    gui/mainwindow.ui

LIBS += -lmcrypt

RESOURCES += \
    res/images.qrc

win32 {
    INCLUDEPATH += ../Libraries/libmcrypt/include
    LIBS += -L../Libraries/libmcrypt/lib
    RC_FILE = res/icon.rc
}

