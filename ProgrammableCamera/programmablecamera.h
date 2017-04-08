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

namespace Ui {
class ProgrammableCamera;
}

class ProgrammableCamera : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProgrammableCamera(QWidget *parent = 0);
    ~ProgrammableCamera();
    HandMode *handMode = NULL;
    ImportMode *importMode = NULL;

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

    QOpenGLWidget *labelCamera1;
    QOpenGLWidget *labelCamera2;

    QSignalMapper *signalsConfig;
    QSignalMapper *signalsHandle;
    QSignalMapper *signalsMode;

    QString selectedConfig; //selected config file name,without '.ini',used to find file path by QMap
    QString selectedHandle; //selected handle file name,used to find file path by QMap

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
};

#endif // PROGRAMMABLECAMERA_H
