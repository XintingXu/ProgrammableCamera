#-------------------------------------------------
#
# Project created by QtCreator 2017-02-20T17:23:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProgrammableCamera
TEMPLATE = app

PRECOMPILED_HEADER = stable.h
QMAKE_CXX = ccache gcc

CONFIG += c++11

unix{
    QMAKE_CXXFLAGS_DEBUG -= -O1
    QMAKE_CXXFLAGS_DEBUG += -O2
    QMAKE_CXXFLAGS_DEBUG -= -O3
    QMAKE_CXXFLAGS_RELEASE -= -O1
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3
}

SOURCES += main.cpp\
        programmablecamera.cpp \
    CameraControl.cpp \
    handmode.cpp \
    importmode.cpp

HEADERS  += programmablecamera.h \
    CameraControl.h \
    handmode.h \
    importmode.h

FORMS    += programmablecamera.ui \
    handmode.ui \
    importmode.ui

LIBS += /usr/lib/arm-linux-gnueabihf/libopencv_highgui.so \
        /usr/lib/arm-linux-gnueabihf/libopencv_core.so    \
        /usr/lib/arm-linux-gnueabihf/libopencv_imgproc.so\
