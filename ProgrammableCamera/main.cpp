#include "programmablecamera.h"
#include "handmode.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ProgrammableCamera w;
    w.show();

    return a.exec();
}
