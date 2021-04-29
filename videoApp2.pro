#-------------------------------------------------
#
# Project created by QtCreator 2021-04-27T16:23:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


#LDFLAGS="-L/opt/ros/kinetic/lib/x86_64-linux-gnu/"
#INCLUDEPATH += /usr/local/include/opencv4/
#LIBS += /usr/local/lib/libopencv*
#LIBS += /opt/ros/kinetic/lib/x86_64-linux-gnu/libopencv_core3.so.3.3

INCLUDEPATH += /opt/ros/kinetic/include/opencv-3.3.1-dev/

LIBS += /opt/ros/kinetic/lib/x86_64-linux-gnu/libopencv*

TARGET = videoApp2
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
