#ifndef CONTROLCAMERACONTROL
#define CONTROLCAMERACONTROL

#include <QThread>
#include <QString>
#include <QSettings>
#include <QImage>
#include <CameraControl.h>
#include <QList>
#include <QVector>
#include <QDebug>
#include <QMap>
#include <QSettings>

using namespace std;

class ControlCameraControl:public QThread{
    Q_OBJECT
public:
    ControlCameraControl();
    ~ControlCameraControl();
    void run();
    bool setConfigFile(QString *);
    bool isExiting;
    bool isCapturing;
    volatile QList<IplImage *> captured[4];
    volatile QList<IplImage *> captureDoneImage;

private:
    QSettings *settingFile;
    CameraControl * camera[4];
    QMap <QString,int> mapOfCameraNumber;
    QString configFileName;
    QList <CameraControlValue*> controlValue[4];
    int haveCapturedNumber[4] = {0,0,0,0};
    QString cameraPath[4] = {"/dev/v4l/by-path/platform-3f980000.usb-usb-0:1.4.2:1.0-video-index0",
                             "/dev/v4l/by-path/platform-3f980000.usb-usb-0:1.4.2:1.0-video-index1",
                             "/dev/v4l/by-path/platform-3f980000.usb-usb-0:1.4.3:1.0-video-index0",
                             "/dev/v4l/by-path/platform-3f980000.usb-usb-0:1.4.3:1.0-video-index1"};
    void setCameraControlValue(CameraControlValue *,QString *,QString *);
    void loadCapture(CameraControl *,int);
    void captureDoneDeal();

private slots:
    void captureFinished(CameraControl *);
    //    void updateUICamera(IplImage *,int);
public slots:
    void startCapture();

signals:
    void handleDone();
    void captureDone(QList<IplImage *>*);
    void logText(QString);
    //    void updateUI(IplImage *,int);
};

#endif // CONTROLCAMERACONTROL
