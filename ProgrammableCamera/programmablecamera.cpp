#include "programmablecamera.h"
#include "ui_programmablecamera.h"

ProgrammableCamera::ProgrammableCamera(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProgrammableCamera)
{
    ui->setupUi(this);

    initUIPointers();

    connect(actionModeHand,SIGNAL(triggered()),this,SLOT(onPressModeHand()));
    connect(actionModeImport,SIGNAL(triggered()),this,SLOT(onPressModeImport()));
    connect(actionQuit,SIGNAL(triggered()),this,SLOT(onPressQuit()));
}

ProgrammableCamera::~ProgrammableCamera()
{
    delete ui;
}

void ProgrammableCamera::initUIPointers(){
    this->MenuMode = ui->menuMode;
    this->MenuConfig = ui->menuConfig;
    this->MenuHandle = ui->menuHandle;
    this->MenuSave = ui->menuSave;
    this->MenuAbout = ui->menuAbout;
    this->MenuQuit = ui->menuQuit;

    this->actionModeHand = ui->actionModeHand;
    this->actionModeImport = ui->actionModeImport;
    this->actionModeHDR = ui->actionModeHDR;
    this->actionModeAll = ui->actionModeAll;
    this->actionModeSingle = ui->actionModeSingle;

    this->actionConfigImport = ui->actionConfigImport;
    this->actionConfigCameras = ui->actionConfigCameras;
    this->actionConfigResolution = ui->actionConfigResolution;
    this->actionConfigExposure = ui->actionConfigExposure;
    this->actionConfigBrightness = ui->actionConfigBrightness;
    this->actionConfigContrast = ui->actionConfigContrast;
    this->actionConfigGamma = ui->actionConfigGamma;

    this->actionHandleImport = ui->actionHandleImport;
    this->actionHandleNO = ui->actionHandleNo;

    this->actionSaveAll = ui->actionSaveAll;
    this->actionSaveResult = ui->actionSaveResult;

    this->actionAboutAuthor = ui->actionAboutAuthor;
    this->actionAboutProject = ui->actionAboutProject;

    this->actionQuit = ui->actionQuit;

    this->labelCamera1 = ui->labelCamera1;
    this->labelCamera2 = ui->labelCamera2;
    this->labelCamera3 = ui->labelCamera3;
    this->labelCamera4 = ui->labelCamera4;

}

void ProgrammableCamera::onPressModeHand(){
    qDebug() << "HandMode pressed.";
    if(!importMode.isActiveWindow())
        handMode.show();
}

void ProgrammableCamera::onPressModeImport(){
    qDebug() << "Import mode pressed.";
    if(!handMode.isActiveWindow())
        importMode.show();
}

void ProgrammableCamera::onPressModeHDR(){
    ;
}

void ProgrammableCamera::onPressModeAll(){
    ;
}

void ProgrammableCamera::onPressModeSingle(){
    ;
}

void ProgrammableCamera::onPressQuit(){
    QApplication::quit();
}
