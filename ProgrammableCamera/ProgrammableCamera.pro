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
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3
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

INCLUDEPATH += /usr/local/opencv-arm/include

if(contains(DEFINES,RPI)){
    LIBS += -L/usr/local/opencv-arm/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lopencv_imgcodecs -lopencv_photo
}else{
    LIBS += -L/usr/local/opencv-pc/lib -lopencv_core -lopencv_highgui -lopencv_imgproc
}
