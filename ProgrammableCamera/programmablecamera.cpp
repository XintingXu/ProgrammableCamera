#include "programmablecamera.h"
#include "ui_programmablecamera.h"

#include <QDir>
#include <QSettings>
#include <QMap>
#include <QVector>
#include <QList>
#include <QMessageBox>
#include <QOpenGLShaderProgram>
#include <QImage>
#include <QSharedMemory>
#include <QBuffer>
#include <QDataStream>
#include <opencv2/opencv.hpp>

extern bool turn_off;

QMap <QString,QString> currentConfig;   //store current config files K-V pair,like "HDR=/home/pi/ProgrammableCamera/config/HDR.ini"
QMap <QString,QString> currentHandle;   //store current handle files K-V pair,like "HDR=/home/pi/ProgrammableCamera/handle/HDR"
QVector <QString> currentMode;          //store current mode names

ProgrammableCamera::ProgrammableCamera(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::ProgrammableCamera)
{
    system("./closeIR.sh");

    selectedConfig = "default";
    selectedHandle = "default";
    selectedSave = "Save All";

    this->isHandling = false;
    this->isExiting = false;
    this->isCapturing = false;

    cameraControl = new ControlCameraControl;
    cameraControl->setParent(this);
    this->CameraViewFinder.push_back(new getShortCut(0));
    //for arm ,use 2
    this->CameraViewFinder.push_back(new getShortCut(2));
    //for PC, use 1
    //this->CameraViewFinder.push_back(new getShortCut(1));
    this->CameraViewFinderTimer.push_back(new QTimer);
    this->CameraViewFinderTimer.push_back(new QTimer);

    for(int i = 0 ; i < 2 ; i++){
        this->CameraViewFinder.at(i)->setParent(this);
        this->CameraViewFinderTimer.at(i)->setParent(this);
    }

    connect(CameraViewFinder.at(0),SIGNAL(updateUI(QImage,int)),this,SLOT(updateViewFinder(QImage,int)),Qt::QueuedConnection);
    connect(CameraViewFinder.at(1),SIGNAL(updateUI(QImage,int)),this,SLOT(updateViewFinder(QImage,int)),Qt::QueuedConnection);
    ui->setupUi(this);

    //Init pointers of UI elements
    initUIPointers();

    logWidget->document()->setMaximumBlockCount(20);

    //connect UI signals with slot functions
    connect(actionModeHand,SIGNAL(triggered()),this,SLOT(onPressModeHand()),Qt::QueuedConnection);
    connect(actionModeImport,SIGNAL(triggered()),this,SLOT(onPressModeImport()),Qt::QueuedConnection);
    connect(actionQuit,SIGNAL(triggered()),this,SLOT(onPressQuit()),Qt::QueuedConnection);
//    connect(this,SIGNAL(startCapture()),cameraControl,SLOT(startCapture()),Qt::QueuedConnection);
    connect(cameraControl,SIGNAL(captureDone(QList<cv::Mat>*)),this,SLOT(onCaptureDone(QList<cv::Mat>*)),Qt::QueuedConnection);
    //connect(cameraControl,SIGNAL(updateUI(cv::Mat*,int)),this,SLOT(updateViewFinder(cv::Mat*,int)));
    //connect(actionPowerOFF,SIGNAL(triggered()),this,SLOT(onPressPowerOFF()));

    signalsConfig = new QSignalMapper(this);
    signalsHandle = new QSignalMapper(this);
    signalsMode = new QSignalMapper(this);
    //search current file path to find any config files or executabale files
    setMenuItems();

    connect(signalsConfig,SIGNAL(mapped(QString)),this,SLOT(onPressConfig(QString)),Qt::QueuedConnection);
    connect(signalsHandle,SIGNAL(mapped(QString)),this,SLOT(onPressHandle(QString)),Qt::QueuedConnection);
    connect(signalsMode,SIGNAL(mapped(QString)),this,SLOT(onPressMode(QString)),Qt::QueuedConnection);

    connect(actionCleanConfigs,SIGNAL(triggered(bool)),this,SLOT(onPressCleanConfig()),Qt::QueuedConnection);
    connect(actionCleanHandles,SIGNAL(triggered(bool)),this,SLOT(onPressCleanHandle()),Qt::QueuedConnection);

    CameraViewFinder.at(0)->start();
    CameraViewFinder.at(1)->start();
    CameraViewFinderTimer.at(0)->start(50);
    CameraViewFinderTimer.at(1)->start(70);
    connect(CameraViewFinderTimer.at(0),SIGNAL(timeout()),this,SLOT(updateViewTimerout0()),Qt::DirectConnection);
    connect(CameraViewFinderTimer.at(1),SIGNAL(timeout()),this,SLOT(updateViewTimerout1()),Qt::DirectConnection);
    connect(buttonIRControl,SIGNAL(clicked(bool)),this,SLOT(onPressButtonIRControl()),Qt::QueuedConnection);
    connect(buttonCapture,SIGNAL(released()),this,SLOT(onPressButtonCapture()),Qt::QueuedConnection);

    handleControlThread = new HandleControl;
    handleControlThread->setParent(this);
    saveControlThread = new SaveControl;
    saveControlThread->setParent(this);
    connect(saveControlThread,SIGNAL(saveDone()),this,SLOT(onSaveDone()),Qt::QueuedConnection);
    connect(handleControlThread,SIGNAL(handleDone()),this,SLOT(onHandleDone()),Qt::QueuedConnection);

    connect(cameraControl,SIGNAL(logText(QString)),this,SLOT(logText(QString)),Qt::QueuedConnection);
}

ProgrammableCamera::~ProgrammableCamera(){
    CameraViewFinderTimer.at(0)->stop();
    CameraViewFinderTimer.at(1)->stop();

//    if(this->cameraControl != NULL){
//        qDebug() << "~ProgrammableCamera : cameraControl : " << cameraControl;
//        delete this->cameraControl;
//        cameraControl = NULL;
//    }
//    //free QMainWindow if it has been opened
//    if(importMode != NULL){
//        qDebug() << "~ProgrammableCamera : importMode : " << importMode;
//        delete importMode;
//        importMode = NULL;
//    }

//    //free QMainWindow if it has been opened
//    if(handMode != NULL){
//        qDebug() << "~ProgrammableCamera : handMode : " << handMode;
//        delete handMode;
//        handMode = NULL;
//    }

//    qDebug() << "~ProgrammableCamera : signalsConfig : " << signalsConfig;
//    delete signalsConfig;
//    qDebug() << "~ProgrammableCamera : signalsHandle : " << signalsHandle;
//    delete signalsHandle;
//    qDebug() << "~ProgrammableCamera : signalsMode : " << signalsMode;
//    delete signalsMode;

    CameraViewFinder.at(0)->wait();
    CameraViewFinder.at(1)->wait();

//    qDebug() << "~ProgrammableCamera : CameraViewFinder.at(0) : " << CameraViewFinder.at(0);
//    getShortCut * toDelete = CameraViewFinder.at(0);
//    delete toDelete;
//    qDebug() << "~ProgrammableCamera : CameraViewFinder.at(1) : " << CameraViewFinder.at(1);
//    toDelete = CameraViewFinder.at(1);
//    delete toDelete;
//    qDebug() << "~ProgrammableCamera : CameraViewFinderTimer.at(0) : " << CameraViewFinderTimer.at(0);
//    QTimer * timerToDelete = CameraViewFinderTimer.at(0);
//    delete timerToDelete;
//    qDebug() << "~ProgrammableCamera : CameraViewFinderTimer.at(1) : " << CameraViewFinderTimer.at(1);
//    timerToDelete = CameraViewFinderTimer.at(1);
//    delete timerToDelete;

//    while (this->addedActions.length() != 0) {
//        if(this->MenuMode->actions().contains(this->addedActions.first())){
//            this->MenuMode->removeAction(this->addedActions.first());
//        }

//        if(this->MenuConfig->actions().contains(this->addedActions.first())){
//            this->MenuConfig->removeAction(this->addedActions.first());
//        }

//        if(this->MenuHandle->actions().contains(this->addedActions.first())){
//            this->MenuHandle->removeAction(this->addedActions.first());
//        }

//        qDebug() << "~ProgrammableCamera : action : " << this->addedActions.first();
//        delete this->addedActions.first();
//        this->addedActions.removeFirst();
//    }

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

    //labels to show images
    this->labelCamera1 = ui->labelCamera1;
    this->labelCamera2 = ui->labelCamera2;

    this->logWidget = ui->textBrowser;

    this->buttonIRControl = ui->pushButtonOpenIR;
    this->buttonCapture = ui->pushButtonCapture;
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
    if(!this->cameraControl->isCapturing){
        this->selectedConfig = name;
        this->logText("choosing config :" + name);
    }else{
        this->logText("System is capturing");
    }
}

void ProgrammableCamera::onPressHandle(QString name){
    qDebug() << "Handle choose : " << name;
    if(!isHandling){
        this->selectedHandle = name;
        this->logText("choosing handle :" + name);
    }else{
        this->logText("System is Handling");
    }
}

void ProgrammableCamera::onPressMode(QString name){
    qDebug() << "Mode choose : " << name;
    if(!isHandling){
        this->selectedConfig = name;
        this->selectedHandle = name;
        this->logText("choosing mode :" + name);
    }else{
        this->logText("System is Handling");
    }
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
        if(system(commandline.toStdString().data()) == 0){
            qDebug() << "Config files has been deleted.";
        }
    }else{
        qDebug() << "Config files will not be deleted.";
    }
    this->setMenuItems();
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
        if(system(commandline.toStdString().data()) == 0){
            qDebug() << "Handle files has been deleted.";
        }
    }else{
        qDebug() << "Handle files will not be deleted.";
    }
    this->setMenuItems();
}

void ProgrammableCamera::onPressQuit(){
    //exit System
    this->isExiting = true;
    turn_off = false;
    this->close();
}

void ProgrammableCamera::onPressPowerOFF(){
    //Power off the system
    turn_off = true;
}

void ProgrammableCamera::updateViewTimerout0(){
    if(!CameraViewFinder.at(0)->isRunning()){
        CameraViewFinder.at(0)->start();
        CameraViewFinderTimer.at(0)->start(40);
    }else{
        CameraViewFinderTimer.at(0)->start(30);
    }
}

void ProgrammableCamera::updateViewTimerout1(){
    if(!CameraViewFinder.at(1)->isRunning()){
        CameraViewFinder.at(1)->start();
        CameraViewFinderTimer.at(1)->start(40);
    }else{
        CameraViewFinderTimer.at(1)->start(30);
    }
}

void ProgrammableCamera::updateViewFinder(QImage image,int cameraNumber){
    threadLock.lock();

    if(image.byteCount() == 0){
        qDebug() << "UI get NULL img pointer.";
    }else{
        if((cameraNumber >> 1) == 0){
            this->labelCamera1->setPixmap(QPixmap::fromImage(image));
        }else{
            this->labelCamera2->setPixmap(QPixmap::fromImage(image));
        }
    }

    threadLock.unlock();
}

void ProgrammableCamera::onCaptureDone(QList<cv::Mat> *captured){
    this->threadLock.lock();
    this->isCapturing = false;
    QString command;
    command.sprintf("rm temp_images/%s*",selectedHandle.toStdString().data());
    system(command.toStdString().data());

    qDebug() << "get " << captured->length() << " images in total";

    for(int i = 0 ; i < captured->length() ; i++){
        cv::Mat toSave;
        captured->at(i).copyTo(toSave);
        QString fileName;
        fileName.sprintf("temp_images/%s%d.jpg",selectedHandle.toStdString().data(),i + 1);
        cv::imwrite(fileName.toStdString().data(),toSave);
    }

    captured->clear();

    this->isCapturing = false;
    logText("capture done");

    command.clear();
    command.sprintf("rm images/%s*",selectedHandle.toStdString().data());
    system(command.toStdString().data());

    this->handleControlThread->setCurrentHandlePath(currentHandle[selectedHandle]);
    this->handleControlThread->start();

    threadLock.unlock();
}

void ProgrammableCamera::onHandleDone(){
    threadLock.lock();

    emit logText("Handle done");
    this->isHandling = false;
    this->saveControlThread->setSaveMode(selectedSave);
    this->saveControlThread->setHandleName(selectedHandle);
    this->saveControlThread->start();

    threadLock.unlock();
}

void ProgrammableCamera::onSaveDone(){
    threadLock.lock();

    emit logText("Save done");
    this->buttonCapture->setEnabled(true);

    CameraViewFinderTimer.at(0)->start(100);
    CameraViewFinderTimer.at(1)->start(150);

    threadLock.unlock();
}

void ProgrammableCamera::onPressButtonIRControl(){
    threadLock.lock();

    if(this->buttonIRControl->text() == "Open IR"){
        qDebug() << "Open IR LED.";

        system("./openIR.sh");

        this->buttonIRControl->setText("Close IR");
        this->buttonIRControl->repaint();
        logText("Open IR LED.");
    }else{
        qDebug() << "Close IR LED.";

        system("./closeIR.sh");

        this->buttonIRControl->setText("Open IR");
        this->buttonIRControl->repaint();
        logText("Close IR LED.");
    }

    threadLock.unlock();
}

void ProgrammableCamera::onPressButtonCapture(){
    threadLock.lock();

    if(this->isCapturing){
        logText("Wait for the former Capture.");
    }else{
        logText("Capture Button is Pressed.");
        this->isCapturing = true;
        this->buttonCapture->setEnabled(false);

        CameraViewFinderTimer.at(0)->stop();
        CameraViewFinderTimer.at(1)->stop();

        CameraViewFinder.at(0)->wait();
        CameraViewFinder.at(1)->wait();

        if(this->cameraControl->setConfigFile(currentConfig[selectedConfig])){
            qDebug() << "ProgrammableCamera start startCapture";
            this->cameraControl->start(QThread::TimeCriticalPriority);
        }else{
            this->buttonCapture->setEnabled(true);
        }
    }
    qDebug() << "Capture Button is Pressed.";
    this->isCapturing = true;

    threadLock.unlock();
}


void ProgrammableCamera::logText(QString textMessage){
    logWidget->append(textMessage);
    logWidget->moveCursor(QTextCursor::End);
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
                currentConfig.insert(list.at(i).baseName().toLower(),list.at(i).absoluteFilePath());
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
                currentHandle.insert(list.at(i).baseName().toLower(),list.at(i).absoluteFilePath());
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
                added->setParent(this);
                signalsConfig->setMapping(added,it.key());
                this->addedActions.push_back(added);
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
                added->setParent(this);
                signalsHandle->setMapping(added,it.key());
                this->addedActions.push_back(added);
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
                    added->setParent(this);
                    signalsMode->setMapping(added,currentMode.at(i));
                    this->addedActions.push_back(added);
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
