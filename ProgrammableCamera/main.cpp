#include "programmablecamera.h"
#include "handmode.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ProgrammableCamera w;
    //w.show();

    w.showFullScreen();

    w.handMode.setParent(&w);
    w.importMode.setParent(&w);

    return a.exec();
}
