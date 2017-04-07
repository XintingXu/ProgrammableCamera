#include "programmablecamera.h"
#include "handmode.h"
#include <QApplication>
#include <unistd.h>
#include <QDir>

int (*myclose)(int) = close;
bool turn_off = true;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    ProgrammableCamera w;
    //w.show();

    w.showFullScreen();

    int result = a.exec();

    if(turn_off){
        system("sudo poweroff");
    }else{
        ;
    }
    return result;
}
