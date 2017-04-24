#include "programmablecamera.h"
#include "handmode.h"
#include <QApplication>
#include <unistd.h>
#include <QDir>
#include <QSurfaceFormat>
#include <wiringPi.h>

int (*myclose)(int) = close;
bool turn_off = true;

int main(int argc, char *argv[])
{
    wiringPiSetup();
    pinMode(22,OUTPUT);
    pinMode(28,INPUT);

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
