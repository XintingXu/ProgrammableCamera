#ifndef IMPORTMODE_H
#define IMPORTMODE_H

#include <QDialog>
#include <QString>
#include <QHash>

namespace Ui {
class ImportMode;
}

class ImportMode : public QDialog
{
    Q_OBJECT

public:
    explicit ImportMode(QWidget *parent = 0);
    ~ImportMode();

private:
    Ui::ImportMode *ui;
};

#endif // IMPORTMODE_H
