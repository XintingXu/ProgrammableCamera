#ifndef PROGRAMMABLECAMERA_H
#define PROGRAMMABLECAMERA_H

#include <QMainWindow>
#include <handmode.h>
#include <importmode.h>
#include <CameraControl.h>
#include <QLabel>
#include <QMap>
#include <QString>
#include <QSignalMapper>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QWidget>
#include <QTextBrowser>
#include <QOpenGLTexture>
#include <controlcameracontrol.h>

//QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

namespace Ui {
class ProgrammableCamera;
}

class GPIOInterruptJudge:public QThread{
    Q_OBJECT
public:
    GPIOInterruptJudge(int ,QSemaphore *);
    void run();
    volatile bool isExiting;
private :
    int pinNumber;
    QSemaphore *judgeSemaphore;
signals:
    void getSignal();
};

class ProgrammableCamera : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProgrammableCamera(QWidget *parent = 0);
    ~ProgrammableCamera();
    HandMode *handMode = NULL;
    ImportMode *importMode = NULL;
    volatile bool isExiting;

private:
    Ui::ProgrammableCamera *ui;

    void initUIPointers();
    void readConfig();
    void readHandle();
    void setMenuItems();

    QMenu *MenuMode;
    QMenu *MenuConfig;
    QMenu *MenuSave;
    QMenu *MenuHandle;
    QMenu *MenuAbout;
    QMenu *MenuQuit;

    QAction *actionModeHand;
    QAction *actionModeImport;

    QAction *actionConfigImport;
    QAction *actionConfigCameras;
    QAction *actionConfigResolution;
    QAction *actionConfigExposure;
    QAction *actionConfigBrightness;
    QAction *actionConfigContrast;
    QAction *actionConfigGamma;

    QAction *actionHandleImport;
    QAction *actionHandleNO;

    QAction *actionSaveAll;
    QAction *actionSaveResult;

    QAction *actionAboutProject;
    QAction *actionAboutAuthor;
    QAction *actionCleanConfigs;
    QAction *actionCleanHandles;

    QAction *actionQuit;
    QAction *actionPowerOFF;

    QLabel *labelCamera1;
    QLabel *labelCamera2;

    QSignalMapper *signalsConfig;
    QSignalMapper *signalsHandle;
    QSignalMapper *signalsMode;

    QTextBrowser *logWidget;

    QString selectedConfig; //selected config file name,without '.ini',used to find file path by QMap
    QString selectedHandle;//selected handle file name,used to find file path by QMap

    volatile bool isHandling;
    volatile bool isCapturing;

    ControlCameraControl *cameraControl;
    QVector <getShortCut *> CameraViewFinder;
    QVector <QTimer *> CameraViewFinderTimer;

    QPushButton *buttonIRControl;
    QPushButton *buttonCapture;

    GPIOInterruptJudge *GPIOINterruptCapture;

private slots:
    void onPressModeHand();
    void onPressModeImport();

    void onPressQuit();
    void onPressPowerOFF();

    void onPressConfig(QString name);
    void onPressHandle(QString name);
    void onPressMode(QString name);

    void onPressCleanConfig();
    void onPressCleanHandle();

    void onPressButtonIRControl();
    void onPressButtonCapture();

    void updateViewFinder(QImage,int);
    void updateViewTimerout0();
    void updateViewTimerout1();

public slots:
    void onHandleDone();
    void logText(QString);
    void onCaptureDone(QList<IplImage*>*);
signals:
    void startCapture();
};

class CameraViewQOpenGLWidget:public QOpenGLWidget,protected QOpenGLFunctions{
    Q_OBJECT
public:
    CameraViewQOpenGLWidget(QWidget *parent = 0);
    ~CameraViewQOpenGLWidget();

protected:
    void initializeGL();
    void paintGL();

private:
    QOpenGLBuffer vbo;
    QOpenGLTexture *texture;

public slots:
    void updateImage(IplImage *);
};

class HandleControl:public QThread{
    Q_OBJECT
public:
    HandleControl();
    ~HandleControl();
    void run();
    void setCurrentHandlePath(QString);
private:
    QString currentHandlePath;
};

class SaveControl:public QThread{
    Q_OBJECT
public:
    SaveControl();
    ~SaveControl();
    void run();
    void setSaveMode(QString);
private:
    int saveMode;
    QMap<QString,int> mapOfMode;
};

#endif // PROGRAMMABLECAMERA_H
