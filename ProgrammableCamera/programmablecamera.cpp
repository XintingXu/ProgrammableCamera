#include "programmablecamera.h"
#include "ui_programmablecamera.h"

#include <QDir>
#include <QSettings>
#include <QMap>
#include <QVector>
#include <QList>
#include <QMessageBox>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>
#include <QOpenGLShaderProgram>
#include <QImage>
#include <QSharedMemory>
#include <QBuffer>
#include <QDataStream>
#include <opencv2/opencv.hpp>
#include <qimagewithiplimage.h>
#include <wiringPi.h>

extern bool turn_off;

QMap <QString,QString> currentConfig;   //store current config files K-V pair,like "HDR=/home/pi/ProgrammableCamera/config/HDR.ini"
QMap <QString,QString> currentHandle;   //store current handle files K-V pair,like "HDR=/home/pi/ProgrammableCamera/handle/HDR"
QVector <QString> currentMode;          //store current mode names

QSemaphore getIntrrupt(0);

void controlInterupt(){
    getIntrrupt.acquire(getIntrrupt.available());
    getIntrrupt.release(1);
    qDebug() << "get Signal from WiringPi";
}

ProgrammableCamera::ProgrammableCamera(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::ProgrammableCamera)
{
    this->isHandling = false;
    this->isExiting = false;
    this->isCapturing = false;

    cameraControl = new ControlCameraControl;
    this->CameraViewFinder.push_back(new getShortCut(0));
    this->CameraViewFinder.push_back(new getShortCut(2));
    this->CameraViewFinderTimer.push_back(new QTimer);
    this->CameraViewFinderTimer.push_back(new QTimer);
    connect(CameraViewFinder.at(0),SIGNAL(updateUI(QImage,int)),this,SLOT(updateViewFinder(QImage,int)));
    connect(CameraViewFinder.at(1),SIGNAL(updateUI(QImage,int)),this,SLOT(updateViewFinder(QImage,int)));

    ui->setupUi(this);

    //Init pointers of UI elements
    initUIPointers();

    logWidget->document()->setMaximumBlockCount(20);

    //connect UI signals with slot functions
    connect(actionModeHand,SIGNAL(triggered()),this,SLOT(onPressModeHand()),Qt::QueuedConnection);
    connect(actionModeImport,SIGNAL(triggered()),this,SLOT(onPressModeImport()),Qt::QueuedConnection);
    connect(actionQuit,SIGNAL(triggered()),this,SLOT(onPressQuit()),Qt::QueuedConnection);
    connect(this,SIGNAL(startCapture()),cameraControl,SLOT(startCapture()),Qt::QueuedConnection);
    connect(cameraControl,SIGNAL(captureDone(QList<IplImage*>*)),this,SLOT(onCaptureDone(QList<IplImage*>*)),Qt::QueuedConnection);
    //connect(cameraControl,SIGNAL(updateUI(IplImage*,int)),this,SLOT(updateViewFinder(IplImage*,int)));
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
    connect(CameraViewFinderTimer.at(0),SIGNAL(timeout()),this,SLOT(updateViewTimerout0()));
    connect(CameraViewFinderTimer.at(1),SIGNAL(timeout()),this,SLOT(updateViewTimerout1()));
    connect(buttonIRControl,SIGNAL(clicked(bool)),this,SLOT(onPressButtonIRControl()),Qt::QueuedConnection);
    connect(buttonCapture,SIGNAL(released()),this,SLOT(onPressButtonCapture()),Qt::QueuedConnection);

    if(wiringPiISR(28,INT_EDGE_RISING,&controlInterupt) < 0){
        logText("Your capture button may init failed.");
    }else{
        qDebug() << "Capture Button init Done.";
    }

    GPIOINterruptCapture = new GPIOInterruptJudge(28,&getIntrrupt);
    connect(GPIOINterruptCapture,SIGNAL(getSignal()),this,SLOT(onPressButtonCapture()),Qt::QueuedConnection);
    GPIOINterruptCapture->start(QThread::TimeCriticalPriority);
}

ProgrammableCamera::~ProgrammableCamera(){
    CameraViewFinderTimer.at(0)->stop();
    CameraViewFinderTimer.at(1)->stop();
    CameraViewFinder.at(0)->exit();
    CameraViewFinder.at(1)->exit();
    GPIOINterruptCapture->isExiting = true;
    getIntrrupt.release();
    GPIOINterruptCapture->exit();

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

    if(this->GPIOINterruptCapture->isRunning()){
        disconnect(GPIOINterruptCapture,SIGNAL(getSignal()),this,SLOT(onPressButtonCapture()));
        getIntrrupt.release(1);
        GPIOINterruptCapture->exit();
    }

    CameraViewFinder.at(0)->wait();
    CameraViewFinder.at(1)->wait();
    GPIOINterruptCapture->wait();

    delete GPIOINterruptCapture;
    delete CameraViewFinder.at(0);
    delete CameraViewFinder.at(1);
    delete CameraViewFinderTimer.at(0);
    delete CameraViewFinderTimer.at(1);
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
    QApplication::quit();
}

void ProgrammableCamera::onPressPowerOFF(){
    //Power off the system
    turn_off = true;
    QApplication::quit();
}

void ProgrammableCamera::onHandleDone(){
    this->isHandling = false;
}

void ProgrammableCamera::updateViewTimerout0(){
    if(!CameraViewFinder.at(0)->isRunning()){
        CameraViewFinder.at(0)->start();
        CameraViewFinderTimer.at(0)->start(40);
    }else{
        CameraViewFinderTimer.at(0)->start(10);
    }
}

void ProgrammableCamera::updateViewTimerout1(){
    if(!CameraViewFinder.at(1)->isRunning()){
        CameraViewFinder.at(1)->start();
        CameraViewFinderTimer.at(1)->start(40);
    }else{
        CameraViewFinderTimer.at(1)->start(10);
    }
}

void ProgrammableCamera::updateViewFinder(QImage image,int cameraNumber){
    if(image.byteCount() == 0){
        qDebug() << "UI get NULL img pointer.";
    }else{
        if((cameraNumber >> 1) == 0){
            this->labelCamera1->setPixmap(QPixmap::fromImage(image));
        }else{
            this->labelCamera2->setPixmap(QPixmap::fromImage(image));
        }
    }
}

void ProgrammableCamera::onCaptureDone(QList<IplImage *> *captured){
    QSharedMemory shareCapture;
    shareCapture.setKey("ProgrammableCameraCaptures");
    shareCapture.lock();
    if(shareCapture.isAttached()){
        shareCapture.detach();
    }

    int size = 0;
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    QDataStream stream(&buffer);
    QList<QImage> images;
    for(int i = 0 ; i < captured->length() ; i++){
        IplImage *img = captured->first();
        QImage * result = IplImageToQImage(img);
        images.push_back(*result);
        captured->removeFirst();
        cvReleaseImage(&img);
    }

    stream << images;
    size = buffer.size();

    if(shareCapture.create(size,QSharedMemory::ReadOnly)){
        if(!shareCapture.lock()){
            qDebug() << "Lock Error :" << shareCapture.errorString();
        }else{
            char * to = (char *)shareCapture.data();
            const char * from = buffer.data().data();
            memcpy(to,from,qMin(shareCapture.size(),size));

            shareCapture.unlock();
        }
    }else{
        qDebug() << "Cannot creat share memory of Captured images." << shareCapture.errorString();
        logText("cannot create share memory");
    }

    this->isCapturing = false;

    this->buttonCapture->setEnabled(true);
    logText("capture done");
}


void ProgrammableCamera::onPressButtonIRControl(){
    if(this->buttonIRControl->text() == "Open IR"){
        qDebug() << "Open IR LED.";

        digitalWrite(22,1);

        this->buttonIRControl->setText("Close IR");
        this->buttonIRControl->repaint();
        logText("Open IR LED.");
    }else{
        qDebug() << "Close IR LED.";

        digitalWrite(22,0);

        this->buttonIRControl->setText("Open IR");
        this->buttonIRControl->repaint();
        logText("Close IR LED.");
    }
}

void ProgrammableCamera::onPressButtonCapture(){
    if(this->isCapturing){
        logText("Wait for the former Capture.");
    }else{
        logText("Capture Button is Pressed.");
        this->isCapturing = true;
        this->buttonCapture->setEnabled(false);
        //emit startCapture();
        this->buttonCapture->setEnabled(true);
    }
    qDebug() << "Capture Button is Pressed.";
    this->isCapturing = false;
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


CameraViewQOpenGLWidget::CameraViewQOpenGLWidget(QWidget *parent):QOpenGLWidget(parent){
    ;
}

CameraViewQOpenGLWidget::~CameraViewQOpenGLWidget(){
    vbo.destroy();
}

void CameraViewQOpenGLWidget::initializeGL(){
    initializeOpenGLFunctions();
    glClearColor(0,0,0,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void CameraViewQOpenGLWidget::paintGL(){
    QColor clearColor = Qt::black;
    glClearColor(clearColor.redF(),clearColor.greenF(),clearColor.blueF(),clearColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void CameraViewQOpenGLWidget::updateImage(IplImage * image){
    if(texture != NULL){
        delete texture;
        texture = NULL;
    }

    texture = new QOpenGLTexture(QImage(image->imageData));
    vbo.create();
    vbo.bind();
    paintGL();
}

GPIOInterruptJudge::GPIOInterruptJudge(int pinNumber, QSemaphore *interruptSemaphore){
    this->pinNumber = pinNumber;
    this->judgeSemaphore = interruptSemaphore;
    this->isExiting = false;
}

void GPIOInterruptJudge::run(){
    qDebug() << "GPIO Interrupt run() is running.";

    while(!this->isExiting){
        this->judgeSemaphore->acquire(1);
        qDebug() << "Interrupt get Signal.";
        emit getSignal();
        this->judgeSemaphore->acquire(judgeSemaphore->available());
    }
}
