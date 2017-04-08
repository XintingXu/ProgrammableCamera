#include "programmablecamera.h"
#include "ui_programmablecamera.h"

#include <QDir>
#include <QSettings>
#include <QMap>
#include <QVector>
#include <QList>
#include <QMessageBox>

extern bool turn_off;

QMap <QString,QString> currentConfig;   //store current config files K-V pair,like "HDR=/home/pi/ProgrammableCamera/config/HDR.ini"
QMap <QString,QString> currentHandle;   //store current handle files K-V pair,like "HDR=/home/pi/ProgrammableCamera/handle/HDR"
QVector <QString> currentMode;          //store current mode names

ProgrammableCamera::ProgrammableCamera(QWidget *parent):
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
    //connect(actionPowerOFF,SIGNAL(triggered()),this,SLOT(onPressPowerOFF()));

    signalsConfig = new QSignalMapper(this);
    signalsHandle = new QSignalMapper(this);
    signalsMode = new QSignalMapper(this);
    //search current file path to find any config files or executabale files
    setMenuItems();

    connect(signalsConfig,SIGNAL(mapped(QString)),this,SLOT(onPressConfig(QString)));
    connect(signalsHandle,SIGNAL(mapped(QString)),this,SLOT(onPressHandle(QString)));
    connect(signalsMode,SIGNAL(mapped(QString)),this,SLOT(onPressMode(QString)));

    connect(actionCleanConfigs,SIGNAL(triggered(bool)),this,SLOT(onPressCleanConfig()));
    connect(actionCleanHandles,SIGNAL(triggered(bool)),this,SLOT(onPressCleanHandle()));
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
    this->actionCleanConfigs = ui->actionCleanConfigs;
    this->actionCleanHandles = ui->actionCleanHandles;

    this->actionQuit = ui->actionQuit;
    this->actionPowerOFF = ui->actionPowerOFF;

    //pointers of OpenGLWidgets to show images
    this->labelCamera1 = ui->openGLWidget0;
    this->labelCamera2 = ui->openGLWidget1;

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
    qDebug() << "Config choose : " << name;
    this->selectedConfig = name;
}

void ProgrammableCamera::onPressHandle(QString name){
    qDebug() << "Handle choose : " << name;
    this->selectedHandle = name;
}

void ProgrammableCamera::onPressMode(QString name){
    qDebug() << "Mode choose : " << name;
    this->selectedConfig = name;
    this->selectedHandle = name;
}

void ProgrammableCamera::onPressCleanConfig(){
    QMessageBox message;
    message.setIcon(QMessageBox::Warning);
    message.setText("Are you sure to clean all the .ini files?");
    message.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    message.setDefaultButton(QMessageBox::Cancel);
    message.setWindowFlags(Qt::WindowStaysOnTopHint);
    int ret = message.exec();
    if(ret == QMessageBox::Yes){
        QString commandline = "rm -rf " + QApplication::applicationDirPath() + "/config/*";
        system(commandline.toStdString().data());
        qDebug() << "Config files has been deleted.";
    }else{
        qDebug() << "Config files will not be deleted.";
    }
}

void ProgrammableCamera::onPressCleanHandle(){
    QMessageBox message;
    message.setIcon(QMessageBox::Warning);
    message.setText("Are you sure to clean all the handle files?");
    message.setStandardButtons(QMessageBox::Cancel | QMessageBox::Yes);
    message.setDefaultButton(QMessageBox::Cancel);
    message.setWindowFlags(Qt::WindowStaysOnTopHint);
    int ret = message.exec();
    if(ret == QMessageBox::Yes){
        QString commandline = "rm -rf " + QApplication::applicationDirPath() + "/handle/*";
        system(commandline.toStdString().data());
        qDebug() << "Handle files has been deleted.";
    }else{
        qDebug() << "Handle files will not be deleted.";
    }
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
        qDebug() << "current config path : " << ConfigDir.absolutePath();
        QStringList filter;
        filter << "*.ini";
        ConfigDir.setNameFilters(filter);   //search only .ini files
        QFileInfoList list = ConfigDir.entryInfoList();
        if(list.length() == 0){
            qDebug() << "current config path cannot find any .ini files.";
        }else{
            for (int i = list.length() - 1 ; i >= 0  ; i--){
                currentConfig.insert(list.at(i).baseName(),list.at(i).absoluteFilePath());
                qDebug() << "find .ini " << list.at(i).baseName();
            }
            qDebug() << "have found : " << list.length() << " .ini files";
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
        qDebug() << "current handle path : " << HandleDir.absolutePath();
        HandleDir.setFilter(QDir::NoSymLinks | QDir::Files | QDir::Executable);
        QFileInfoList list = HandleDir.entryInfoList();
        if(list.length() == 0){
            qDebug() << "current handle path cannot find any executable files.";
        }else{
            for (int i = list.length() - 1 ; i >= 0 ; i--){
                currentHandle.insert(list.at(i).baseName(),list.at(i).absoluteFilePath());
                qDebug() << "find handle " << list.at(i).baseName();
            }
            qDebug() << "have found : " << list.length() << " handle files";
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

            for(int i = find.length() - 1 ; i >= 0  ; i--){   //check if items was already in the menu
                if(find.at(i)->text() == it.key()){
                    haveFound = true;
                    break;
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
            for(int i = find.length() - 1 ; i >= 0  ; i--){   //check if items was already in the menu
                if(find.at(i)->text() == it.key()){
                    haveFound = true;
                    break;
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
            for(it = currentHandle.begin() ; it != currentHandle.end() ; it++){
                if(currentConfig.find(it.key()) != currentConfig.end()){
                    currentMode.append(it.key());
                }else{
                    ;
                }
            }
        }else{
            for(it = currentConfig.begin() ; it != currentConfig.end() ; it++){
                if(currentHandle.find(it.key()) != currentHandle.end()){
                    currentMode.append(it.key());
                }else{
                    ;
                }
            }
        }

        if(!currentMode.empty()){
            QList <QAction*> find = MenuMode->actions();

            for(int i = currentMode.length() - 1 ; i >= 0  ; i--){
                bool haveFound = false;
                for(int j = find.length() - 1 ; j >= 0 ; j--){
                    QAction * findThis = find.at(j);
                    if(findThis->text() == currentMode.at(i)){  //check if items was already in the menu
                        haveFound = true;
                        break;
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
            qDebug() << "There isn't any mode to use.";
        }
    }else{
        qDebug() << "There is no config and handle files";
    }
}
