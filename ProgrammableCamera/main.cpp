#include "programmablecamera.h"
#include "handmode.h"
#include <QApplication>
#include <unistd.h>
#include <QDir>
#include <QSurfaceFormat>
#include <wiringPi.h>
#include <iostream>

int (*myclose)(int) = close;
bool turn_off = true;

int main(int argc, char *argv[])
{
    if(wiringPiSetup() < 0){
        std::cout << "WiringPi Setup Error!" << std::cout;
    }
    pinMode(22,OUTPUT);
    pinMode(28,INPUT);
    digitalWrite(28,0);
    digitalWrite(22,0);

    QApplication a(argc, argv);
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    ProgrammableCamera w;
    //w.show();

    w.showFullScreen();

    int result = a.exec();

    digitalWrite(22,0);

    if(turn_off){
        qDebug() << "System call(poweroff) returns :" << system("sudo poweroff");
    }else{
        ;
    }
    return result;
}
