#ifndef PROGRAMMABLECAMERA_H
#define PROGRAMMABLECAMERA_H

#include <QMainWindow>

namespace Ui {
class ProgrammableCamera;
}

class ProgrammableCamera : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProgrammableCamera(QWidget *parent = 0);
    ~ProgrammableCamera();

private:
    Ui::ProgrammableCamera *ui;
};

#endif // PROGRAMMABLECAMERA_H
