#-------------------------------------------------
#
# Project created by QtCreator 2017-02-20T17:23:24
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProgrammableCamera
TEMPLATE = app

INSTALLS        = target
target.files    = ProgrammableCamera
target.path     = /home/pi/ProgrammableCamera

CONFIG += c++14

unix{
    QMAKE_CXXFLAGS_DEBUG += -O1
    QMAKE_CXXFLAGS_DEBUG -= -O2
    QMAKE_CXXFLAGS_DEBUG -= -O3
    QMAKE_CXXFLAGS_RELEASE -= -O1
    QMAKE_CXXFLAGS_RELEASE += -O2
    QMAKE_CXXFLAGS_RELEASE -= -O3
}

SOURCES += main.cpp\
        programmablecamera.cpp \
    CameraControl.cpp \
    handmode.cpp \
    importmode.cpp \
    controlcameracontrol.cpp \
    programmablecameracontrol.cpp \
    qimagewithiplimage.cpp

HEADERS  += programmablecamera.h \
    CameraControl.h \
    handmode.h \
    importmode.h \
    controlcameracontrol.h \
    qimagewithiplimage.h

FORMS    += programmablecamera.ui \
    handmode.ui \
    importmode.ui

INCLUDEPATH += $$[QT_SYSROOT]/usr/local/include
INCLUDEPATH += $$[QT_SYSROOT]/usr/include
INCLUDEPATH += $$[QT_SYSROOT]/usr/opencv-arm/include

if(contains(DEFINES,RPI)){
    LIBS += /usr/local/opencv-arm/lib/libopencv_*
    LIBS += -L$$[QT_SYSROOT]/usr/lib -lwiringPi
}else{
    LIBS += /usr/local/lib/libopencv_*
}
