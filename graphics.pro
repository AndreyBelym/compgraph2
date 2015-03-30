#-------------------------------------------------
#
# Project created by QtCreator 2015-03-03T13:20:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = graphics
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -D_USE_MATH_DEFINES
SOURCES += main.cpp\
    mainwindow.cpp \
    compgraphview.cpp \
    optionswidget.cpp

HEADERS  += \
    model.h \
    graphics.h \
    mainwindow.h \
    compgraphview.h \
    optionswidget.h

FORMS    += \
    mainwindow.ui
