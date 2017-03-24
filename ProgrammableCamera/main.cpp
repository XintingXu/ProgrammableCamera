#include "programmablecamera.h"
#include "handmode.h"
#include <QApplication>
#include <unistd.h>

int (*myclose)(int) = close;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ProgrammableCamera w;
    //w.show();

    w.showFullScreen();

    return a.exec();
}
