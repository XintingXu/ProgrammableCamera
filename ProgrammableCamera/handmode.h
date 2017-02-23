#ifndef HANDMODE_H
#define HANDMODE_H

#include <QDialog>

namespace Ui {
class HandMode;
}

class HandMode : public QDialog
{
    Q_OBJECT

public:
    explicit HandMode(QWidget *parent = 0);
    ~HandMode();

private:
    Ui::HandMode *ui;
};

#endif // HANDMODE_H
