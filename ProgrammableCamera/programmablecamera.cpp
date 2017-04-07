#include "programmablecamera.h"
#include "ui_programmablecamera.h"

#include <QDir>
#include <QSettings>
#include <QMap>
#include <QVector>
#include <QList>

extern bool turn_off;

QMap <QString,QString> currentConfig;   //store current config files K-V pair,like "HDR=/home/pi/ProgrammableCamera/config/HDR.ini"
QMap <QString,QString> currentHandle;   //store current handle files K-V pair,like "HDR=/home/pi/ProgrammableCamera/handle/HDR"
QVector <QString> currentMode;          //store current mode names
QSettings *cameraSetting[5];    //store camera seetings

ProgrammableCamera::ProgrammableCamera(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProgrammableCamera)
{
    ui->setupUi(this);

    //Init pointers of UI elements
    initUIPointers();

    //connect UI signals with slot functions
    connect(actionModeHand,SIGNAL(triggered()),this,SLOT(onPressModeHand()));
    connect(actionModeImport,SIGNAL(triggered()),this,SLOT(onPressModeImport()));
    connect(actionQuit,SIGNAL(triggered()),this,SLOT(onPressQuit()));
    connect(actionPowerOFF,SIGNAL(triggered()),this,SLOT(onPressPowerOFF()));

    signalsConfig = new QSignalMapper(this);
    signalsHandle = new QSignalMapper(this);
    signalsMode = new QSignalMapper(this);
    //search current file path to find any config files or executabale files
    setMenuItems();

    connect(signalsConfig,SIGNAL(mapped(QString)),this,SLOT(onPressConfig(QString)));
    connect(signalsHandle,SIGNAL(mapped(QString)),this,SLOT(onPressHandle(QString)));
    connect(signalsMode,SIGNAL(mapped(QString)),this,SLOT(onPressMode(QString)));
}

ProgrammableCamera::~ProgrammableCamera(){
    //free QMainWindow if it has been opened
    if(importMode != NULL){
        delete importMode;
        importMode = NULL;
    }

    //free QMainWindow if it has been opened
    if(handMode != NULL){
        delete handMode;
        handMode = NULL;
    }

    delete signalsConfig;
    delete signalsHandle;
    delete signalsMode;
    delete ui;
}

void ProgrammableCamera::initUIPointers(){
    //pointers of Menu Collections
    this->MenuMode = ui->menuMode;
    this->MenuConfig = ui->menuConfig;
    this->MenuHandle = ui->menuHandle;
    this->MenuSave = ui->menuSave;
    this->MenuAbout = ui->menuAbout;
    this->MenuQuit = ui->menuQuit;

    //pointers of Menu items
    this->actionModeHand = ui->actionModeHand;
    this->actionModeImport = ui->actionModeImport;

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
    this->actionPowerOFF = ui->actionPowerOFF;

    //pointers of labels to show images
    this->labelCamera1 = ui->labelCamera1;
    this->labelCamera2 = ui->labelCamera2;
}

void ProgrammableCamera::onPressModeHand(){
    //call the QMainWindow of handmode to set arguments
    handMode = new HandMode;
    handMode->setParent(this);
    qDebug() << "HandMode pressed.";
    handMode->showFullScreen();
    handMode->setWindowModality(Qt::ApplicationModal);
}

void ProgrammableCamera::onPressModeImport(){
    //call the QMainWindow of importmode to choose import items
    importMode = new ImportMode;
    importMode->setParent(this);
    qDebug() << "Import mode pressed.";
    importMode->showFullScreen();
    importMode->setWindowModality(Qt::ApplicationModal);
    this->setMenuItems();
}

void ProgrammableCamera::onPressConfig(QString name){
    qDebug() << "Config choose : " << name << endl;
}

void ProgrammableCamera::onPressHandle(QString name){
    qDebug() << "Handle choose : " << name << endl;
}

void ProgrammableCamera::onPressMode(QString name){
    qDebug() << "Mode choose : " << name << endl;
}

void ProgrammableCamera::onPressQuit(){
    //exit System
    turn_off = false;
    QApplication::quit();
}

void ProgrammableCamera::onPressPowerOFF(){
    //Power off the system
    turn_off = true;
    QApplication::quit();
}


void ProgrammableCamera::readConfig(){
    //search config files in current path
    QDir ConfigDir(QApplication::applicationDirPath() + "/config/"); //search config/ path to find .ini files
    if(ConfigDir.exists()){
        qDebug() << "current config path : " << ConfigDir.absolutePath() << endl;
        QStringList filter;
        filter << "*.ini";
        ConfigDir.setNameFilters(filter);   //search only .ini files
        QFileInfoList list = ConfigDir.entryInfoList();
        if(list.length() == 0){
            qDebug() << "current config path cannot find any .ini files." << endl;
        }else{
            for (int i = 0; i < list.length() ; i ++){
                currentConfig.insert(list.at(i).baseName(),list.at(i).absoluteFilePath());
            }
            qDebug() << "have found : " << list.length() << " .ini files" << endl;
        }
    }else{
        ConfigDir = QDir(QApplication::applicationDirPath());   //if there isn't dir,then make it
        ConfigDir.mkdir("config");
    }
}

void ProgrammableCamera::readHandle(){
    //search executable files in current path
    QDir HandleDir(QApplication::applicationDirPath() + "/handle/"); //search only handle/path to find executable files
    if(HandleDir.exists()){
        qDebug() << "current handle path : " << HandleDir.absolutePath() << endl;
        HandleDir.setFilter(QDir::NoSymLinks | QDir::Files | QDir::Executable);
        QFileInfoList list = HandleDir.entryInfoList();
        if(list.length() == 0){
            qDebug() << "current handle path cannot find any executable files." << endl;
        }else{
            for (int i = 0; i < list.length() ; i ++){
                currentHandle.insert(list.at(i).baseName(),list.at(i).absoluteFilePath());
            }
            qDebug() << "have found : " << list.length() << " handle files" << endl;
        }
    }else{
        HandleDir = QDir(QApplication::applicationDirPath());   //if there isn't any handle dirs,make it
        HandleDir.mkdir("handle");
    }
}

void ProgrammableCamera::setMenuItems(){
    //add menu items
    readConfig();
    readHandle();
    if(!currentConfig.isEmpty()){
        QMap<QString,QString>::iterator it;
        QList<QAction*> find = MenuConfig->actions();

        for(it = currentConfig.begin() ; it != currentConfig.end() ; it++){
            bool haveFound = false;

            for(int i = 0 ; i < find.length() ; i++){   //check if items was already in the menu
                if(find.at(i)->text() == it.key()){
                    haveFound = true;
                }else{
                    ;
                }
            }
            if(!haveFound){
                QAction *added = MenuConfig->addAction(it.key());
                signalsConfig->setMapping(added,it.key());
                connect(added,SIGNAL(triggered()),signalsConfig,SLOT(map()));
            }else{
                ;
            }
        }
    }

    if(!currentHandle.isEmpty()){
        QMap<QString,QString>::iterator it;
        QList<QAction*> find = MenuHandle->actions();

        for(it = currentHandle.begin() ; it != currentHandle.end() ; it++){
            bool haveFound = false;

            for(int i = 0 ; i < find.length() ; i++){   //check if items was already in the menu
                if(find.at(i)->text() == it.key()){
                    haveFound = true;
                }else{
                    ;
                }
            }

            if(!haveFound){
                QAction *added = MenuHandle->addAction(it.key());
                signalsHandle->setMapping(added,it.key());
                connect(added,SIGNAL(triggered()),signalsHandle,SLOT(map()));
            }else{
                ;
            }
        }
    }

    if(!currentConfig.empty() && !currentHandle.empty()){
        QMap <QString,QString>::iterator it;
        if(currentConfig.count() > currentHandle.count()){
            for(it = currentHandle.begin() ; it != currentHandle.end() ; it ++){
                if(currentConfig.find(it.key()) != currentConfig.end()){
                    currentMode.append(it.key());
                }else{
                    ;
                }
            }
        }else{
            for(it = currentConfig.begin() ; it != currentConfig.end() ; it ++){
                if(currentHandle.find(it.key()) != currentHandle.end()){
                    currentMode.append(it.key());
                }else{
                    ;
                }
            }
        }

        if(!currentMode.empty()){
            QList <QAction*> find = MenuMode->actions();

            for(int i = 0; i < currentMode.length() ; i++){
                bool haveFound = false;
                for(int j = 0 ; j < find.length() ; j ++){
                    QAction * findThis = find.at(j);
                    if(findThis->text() == currentMode.at(i)){  //check if items was already in the menu
                        haveFound = true;
                    }else{
                        ;
                    }
                }
                if(!haveFound){
                    QAction * added = MenuMode->addAction(currentMode.at(i));
                    signalsMode->setMapping(added,currentMode.at(i));
                    connect(added,SIGNAL(triggered()),signalsMode,SLOT(map()));
                }
            }
        }else{
            qDebug() << "There isn't any mode to use." << endl;
        }
    }else{
        qDebug() << "There is no config and handle files" << endl;
    }
}
