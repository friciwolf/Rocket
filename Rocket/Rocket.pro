#-------------------------------------------------
#
# Project created by QtCreator 2021-04-04T22:20:17
#
#-------------------------------------------------

QT       += core gui KIOCore KIOFileWidgets KIOWidgets KNTLM KI18n KConfigCore KConfigGui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Rocket
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    pager/pager.cpp \
    pager/pageritem.cpp \
    pager/pagercircularindicator.cpp \
    pager/pagercircularactiveindicator.cpp \
    icongrid/icongrid.cpp \
    icongrid/icongriditem.cpp \
    icongrid/icongriditemcanvas.cpp \
    icongrid/kmenuitems.cpp \
    icongrid/kapplication.cpp \
    stylingparams.cpp \
    searchfield/searchfield.cpp \
    tools/searchingapps.cpp \
    tools/rocketconfigmanager.cpp \
    pager/verticalpager.cpp

HEADERS += \
        mainwindow.h \
    pager/pager.h \
    pager/pageritem.h \
    pager/pagercircularindicator.h \
    pager/pagercircularactiveindicator.h \
    icongrid/icongrid.h \
    icongrid/icongriditem.h \
    icongrid/icongriditemcanvas.h \
    icongrid/kmenuitems.h \
    icongrid/kapplication.h \
    stylingparams.h \
    searchfield/searchfield.h \
    tools/searchingapps.h \
    tools/rocketconfigmanager.h \
    pager/verticalpager.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
