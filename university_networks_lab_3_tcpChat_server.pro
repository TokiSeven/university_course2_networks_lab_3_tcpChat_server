#-------------------------------------------------
#
# Project created by QtCreator 2016-02-22T23:02:27
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = university_networks_lab_3_tcpChat_server
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tcpchat_server.cpp

HEADERS  += mainwindow.h \
    tcpchat_server.h

FORMS    += mainwindow.ui
