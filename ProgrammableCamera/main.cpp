#include "programmablecamera.h"
#include "handmode.h"
#include <QApplication>
#include <unistd.h>
#include <QDir>
#include <QSurfaceFormat>
#include <iostream>

int (*myclose)(int) = close;
bool turn_off = false;

int main(int argc, char *argv[])
{
    system("./closeIR.sh");

    QApplication a(argc, argv);
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    ProgrammableCamera w;
    //w.show();

    w.showFullScreen();

    int result = a.exec();

    if(turn_off){
        qDebug() << "System call(poweroff) returns :" << system("sudo poweroff");
    }else{
        ;
    }
    return result;
}
