#-------------------------------------------------
#
# Project created by QtCreator 2015-08-25T14:17:16
#
#-------------------------------------------------

QT       += core gui network webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RFCBrowser
TEMPLATE = app

RC_FILE += RFCBrowser.rc

SOURCES += main.cpp\
        mainwindow.cpp \
    tabbody.cpp \
    filesavemanager.cpp

HEADERS  += mainwindow.h \
    tabbody.h \
    filesavemanager.h

FORMS    += mainwindow.ui \
    tabbody.ui

RESOURCES += \
    png.qrc
