#include "programmablecamera.h"
#include "ui_programmablecamera.h"

#include <QDir>

QString USBpath;
QMap <QString,QString> readConfigName;
QMap <QString,QString> readHandleName;
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

//读取U盘中存在的配置文件以及处理文件
void ModeRead::run(){
    QDir USBDir("/media/pi/");		//USB可挂载设备的母文件夹
    if(!USBDir.exists()){
        qDebug() << "Errors about the USB FileSystem" << endl;
    }else{
        USBDir.setFilter(QDir::Dirs | QDir::NoSymLinks);		//过滤文件夹，不包括符号链接
        USBDir.setSorting(QDir::Size);							//排序

        QFileInfoList list = USBDir.entryInfoList();
/*
        for (int i = 0 ; i < list.size() ; i ++){
            qDebug() << "i = " << i << " : " << list.at(i).absoluteFilePath() << endl;
        }
*/
        QFileInfo fileInfo = list.at(0);						//读取USB盘名

        USBpath = fileInfo.absoluteFilePath();					//得到绝对路径
//        qDebug() << USBpath << endl;

        USBDir = QDir(USBpath);

        if(USBDir.exists()){
            qDebug() << "USB device read OK." << endl;// USBDir << endl;
            USBDeviceOK = true;

            QString temp = USBpath;
            USBDir = QDir(temp.append("/config/"));				//进入到config文件夹，准备读取配置文件

            if(!USBDir.exists()){
                qDebug() << USBDir << endl;
                qDebug() << "Could not find config folder." << endl;	//没有找到配置文件
            }else{
                QStringList filter;		//过滤.ini类型的文件
                filter << "*.ini";
                USBDir.setNameFilters(filter);

                list = USBDir.entryInfoList();		//得到过滤结果

                if(list.count() == 0){		//没有读到.ini文件
                    qDebug() << "Cannot find any .ini file." << endl;
                }else{
                    qDebug() << "Found .ini file." << endl;

                    for(int i = 0 ; i < list.count() ; i++){
                        readConfigName.insert(list.at(i).completeBaseName(),list.at(i).absoluteFilePath());		//只读取文件名，不读配置名
                        qDebug() << list.at(i).completeBaseName() << " : " << readConfigName.value(list.at(i).completeBaseName()) << endl;
                    }

                    temp = USBpath;
                    USBDir = QDir(temp.append("/handle/"));			//进入handle文件夹，准备读取处理程序

                    if(!USBDir.exists()){
                        qDebug() << USBDir << endl;
                        qDebug() << "Could not find handle folder." << endl;		//未找到handle文件夹
                    }else{
                        USBDir.setFilter(QDir::NoSymLinks | QDir::Files);			//过滤，只保留文件，去除符号链接文件
                        USBDir.setSorting(QDir::Name);

                        list = USBDir.entryInfoList();

                        if(list.count() == 0){
                            qDebug() << "Cannot find any executable file." << endl;
                        }else{
                            qDebug() << "Found Executable file." << endl;

                            for(int i = 0; i < list.count() ; i++){
                                readHandleName.insert(list.at(i).completeBaseName(),list.at(i).absoluteFilePath());		//读取处理文件，包含拓展格式
                                qDebug() << list.at(i).completeBaseName() << " : " << readHandleName.value(list.at(i).completeBaseName()) << endl;
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
