#ifndef PROGRAMMABLECAMERA_H
#define PROGRAMMABLECAMERA_H

#include <QMainWindow>
#include <handmode.h>
#include <importmode.h>
#include <CameraControl.h>
#include <QLabel>
#include <QMap>
#include <QString>

namespace Ui {
class ProgrammableCamera;
}

class ProgrammableCamera : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProgrammableCamera(QWidget *parent = 0);
    ~ProgrammableCamera();
    HandMode *handMode;
    ImportMode *importMode;

private:
    Ui::ProgrammableCamera *ui;

    void initUIPointers();

    QMenu *MenuMode;
    QMenu *MenuConfig;
    QMenu *MenuSave;
    QMenu *MenuHandle;
    QMenu *MenuAbout;
    QMenu *MenuQuit;

    QAction *actionModeHand;
    QAction *actionModeImport;
    QAction *actionModeHDR;
    QAction *actionModeAll;
    QAction *actionModeSingle;

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

    QAction *actionQuit;

    QLabel *labelCamera1;
    QLabel *labelCamera2;
    QLabel *labelCamera3;
    QLabel *labelCamera4;

private slots:
    void onPressModeHand();
    void onPressModeImport();
    void onPressModeHDR();
    void onPressModeAll();
    void onPressModeSingle();

    void onPressQuit();
};

#endif // PROGRAMMABLECAMERA_H
