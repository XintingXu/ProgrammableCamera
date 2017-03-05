#include "importmode.h"
#include "ui_importmode.h"

#include "programmablecamera.h"

extern QMap <int,QString> readConfigName;
extern QMap <int,QString> readHandleName;


ImportMode::ImportMode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportMode)
{
    for(int i = 0 ; i < readConfigName.count() ; i ++){
        ;
    }

    ui->setupUi(this);
}

ImportMode::~ImportMode()
{
    delete ui;
}
