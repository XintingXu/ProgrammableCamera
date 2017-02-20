#include "programmablecamera.h"
#include "ui_programmablecamera.h"

ProgrammableCamera::ProgrammableCamera(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProgrammableCamera)
{
    ui->setupUi(this);
}

ProgrammableCamera::~ProgrammableCamera()
{
    delete ui;
}
