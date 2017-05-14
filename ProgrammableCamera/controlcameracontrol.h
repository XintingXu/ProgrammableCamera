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
#include <QMutex>

using namespace std;

class ControlCameraControl:public QThread{
    Q_OBJECT
public:
    ControlCameraControl();
    ~ControlCameraControl();
    void run();
    bool setConfigFile(QString );
    bool isExiting;
    bool isCapturing;
    QList<cv::Mat> *captured[4];
    QList<cv::Mat> captureDoneImage;

private:
    QMutex threadLock;
    QSettings *settingFile;
    CameraControl * camera[4];
    QMap <QString,int> mapOfCameraNumber;
    QString configFileName;
    QList <CameraControlValue> controlValue[4];
    QString cameraPath[4] = {"/dev/v4l/by-path/platform-3f980000.usb-usb-0:1.4.2:1.0-video-index0",
                             "/dev/v4l/by-path/platform-3f980000.usb-usb-0:1.4.2:1.0-video-index1",
                             "/dev/v4l/by-path/platform-3f980000.usb-usb-0:1.4.3:1.0-video-index0",
                             "/dev/v4l/by-path/platform-3f980000.usb-usb-0:1.4.3:1.0-video-index1"};
    void setCameraControlValue(CameraControlValue *,QString *,QString *);
    void loadCapture(CameraControl *,int);
    void captureDoneDeal();

public slots:
    void startCapture();

signals:
    void captureDone(QList<cv::Mat>*);
    void logText(QString);
};

#endif // CONTROLCAMERACONTROL
