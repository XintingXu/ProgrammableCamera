#ifndef IMPORTMODE_H
#define IMPORTMODE_H

#include <QDialog>
#include <QString>
#include <QHash>
#include <QThread>
#include <QVector>

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

private slots:
    void acceptPressed();
    void rejectPressed();
};


class ModeCheck : public QThread{
  Q_OBJECT
public:
    void run();
};

#endif // IMPORTMODE_H
