#include "importmode.h"
#include "ui_importmode.h"

ImportMode::ImportMode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportMode)
{
    ui->setupUi(this);
}

ImportMode::~ImportMode()
{
    delete ui;
}
