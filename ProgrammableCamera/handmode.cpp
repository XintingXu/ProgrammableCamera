#include "handmode.h"
#include "ui_handmode.h"

HandMode::HandMode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HandMode)
{
    ui->setupUi(this);
}

HandMode::~HandMode()
{
    delete ui;
}
