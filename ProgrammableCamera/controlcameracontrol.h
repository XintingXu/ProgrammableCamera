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

using namespace std;

class ControlCameraControl:public QThread{
    Q_OBJECT
public:
    ControlCameraControl();
    ~ControlCameraControl();
    void run();
    bool setConfigFile(QString *);

private:
    CameraControl * camera0;
    CameraControl * camera1;
    QString configFileName;
    QList <CameraControlValue*> controlValue0;
    QList <CameraControlValue*> controlValue1;

private slots:
    void captureFinished(CameraControl *);
};

#endif // CONTROLCAMERACONTROL
