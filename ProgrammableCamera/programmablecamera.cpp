#include "programmablecamera.h"
#include "ui_programmablecamera.h"

#include <QDir>

QString USBpath;
QMap <int,QString> readConfigName;
QMap <int,QString> readHandleName;
bool USBDeviceOK = false;

ModeRead *modeRead;

ProgrammableCamera::ProgrammableCamera(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProgrammableCamera)
{
    ui->setupUi(this);

    modeRead = new ModeRead;
    modeRead->start();
    initUIPointers();

    connect(actionModeHand,SIGNAL(triggered()),this,SLOT(onPressModeHand()));
    connect(actionModeImport,SIGNAL(triggered()),this,SLOT(onPressModeImport()));
    connect(actionQuit,SIGNAL(triggered()),this,SLOT(onPressQuit()));

    modeRead->wait();
}

ProgrammableCamera::~ProgrammableCamera()
{
    delete modeRead;
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
    //if(!importMode.isActiveWindow())
        handMode.show();
}

void ProgrammableCamera::onPressModeImport(){
    qDebug() << "Import mode pressed.";
    //if(!handMode.isActiveWindow())
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


void ModeRead::run(){
    QDir USBDir("/media/pi/");
    if(!USBDir.exists()){
        qDebug() << "Errors about the USB FileSystem" << endl;
    }else{
        USBDir.setFilter(QDir::Dirs | QDir::NoSymLinks);
        USBDir.setSorting(QDir::Size);

        QFileInfoList list = USBDir.entryInfoList();
/*
        for (int i = 0 ; i < list.size() ; i ++){
            qDebug() << "i = " << i << " : " << list.at(i).absoluteFilePath() << endl;
        }
*/
        QFileInfo fileInfo = list.at(0);

        USBpath = fileInfo.absoluteFilePath();
//        qDebug() << USBpath << endl;

        USBDir = QDir(USBpath);

        if(USBDir.exists()){
            qDebug() << "USB device read OK." << endl;// USBDir << endl;
            USBDeviceOK = true;

            QString temp = USBpath;
            USBDir = QDir(temp.append("/config/"));

            if(!USBDir.exists()){
                qDebug() << USBDir << endl;
                qDebug() << "Could not find config folder." << endl;
            }else{
                QStringList filter;
                filter << "*.ini";
                USBDir.setNameFilters(filter);

                list = USBDir.entryInfoList();

                if(list.count() == 0){
                    qDebug() << "Cannot find any .ini file." << endl;
                }else{
                    qDebug() << "Found .ini file." << endl;

                    for(int i = 0 ; i < list.count() ; i++){
                        readConfigName.insert(i,list.at(i).completeBaseName());
                        //qDebug() << readConfigName.value(i) << endl;
                    }

                    temp = USBpath;
                    USBDir = QDir(temp.append("/handle/"));

                    if(!USBDir.exists()){
                        qDebug() << USBDir << endl;
                        qDebug() << "Could not find handle folder." << endl;
                    }else{
                        USBDir.setFilter(QDir::NoSymLinks | QDir::Files);
                        USBDir.setSorting(QDir::Name);

                        list = USBDir.entryInfoList();

                        if(list.count() == 0){
                            qDebug() << "Cannot find any executable file." << endl;
                        }else{
                            qDebug() << "Found Executable file." << endl;

                            for(int i = 0; i < list.count() ; i++){
                                readHandleName.insert(i,list.at(i).fileName());
                                //qDebug() << readHandleName.value(i) << endl;
                            }

                            qDebug() << "Config and Handle files read done" << endl;
                        }
                    }
                }
            }

        }else{
            qDebug() << "USB device is not reachable." << endl;
        }
    }
}
