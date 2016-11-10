#-------------------------------------------------
#
# Project created by QtCreator 2016-07-23T22:37:11
#
#-------------------------------------------------

QT       += core gui sql charts printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 786Plastics
TEMPLATE = app


SOURCES += main.cpp\
    factory.cpp \
    paymentdialog.cpp \
    selldialog.cpp \
    buydialog.cpp \
    logindialog.cpp \
    settingsdialog.cpp \
    dealermodel.cpp \
    stockmodel.cpp \
    salesmodel.cpp \
    paymentmodel.cpp \
    application.cpp \
    reportsdialog.cpp \
    tableprinter.cpp

HEADERS  += \
    factory.h \
    paymentdialog.h \
    selldialog.h \
    buydialog.h \
    logindialog.h \
    settingsdialog.h \
    dealermodel.h \
    stockmodel.h \
    salesmodel.h \
    paymentmodel.h \
    application.h \
    reportsdialog.h \
    tableprinter.h

FORMS    += \
    factory.ui \
    paymentdialog.ui \
    selldialog.ui \
    buydialog.ui \
    logindialog.ui \
    settingsdialog.ui \
    reportsdialog.ui

RESOURCES += \
    resource.qrc
