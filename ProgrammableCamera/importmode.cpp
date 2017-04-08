#include "importmode.h"
#include "ui_importmode.h"

#include <QDir>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QAbstractButton>
#include <QPushButton>

QMap <QString,QString> readConfigName;
QMap <QString,QString> readHandleName;
QString USBpath;
bool USBDeviceOK = false;


ImportMode::ImportMode(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportMode)
{
    ModeCheck modeCheck;
    modeCheck.start();
    modeCheck.wait();

    ui->setupUi(this);

    connect(ui->buttonBox,SIGNAL(accepted()),this,SLOT(acceptPressed()));
    connect(ui->buttonBox,SIGNAL(rejected()),this,SLOT(rejectPressed()));

    QMap <QString,QString> ::iterator it;
    for(it = readConfigName.begin() ; it != readConfigName.end() ; it++){
        if(readHandleName.find(it.key()) != readHandleName.end()){
            ui->listWidget->addItem(it.key());
        }
    }
}

ImportMode::~ImportMode()
{
    delete ui;
}

void ImportMode::acceptPressed(){
    qDebug() << "Accept Button pressed in ImportMode UI.";

    QList <QListWidgetItem*>selectedItem = ui->listWidget->selectedItems();
    if(selectedItem.length() == 0){
        qDebug() << "No mode has been choosen.";
    }else{
        QFile configFile(readConfigName[selectedItem.at(0)->text()]);
        qDebug() << "QFile Config.name : " << configFile.fileName();
        QFile handleFile(readHandleName[selectedItem.at(0)->text()]);

        QFile targetConfigFile(QApplication::applicationDirPath() + "/config/" + selectedItem.at(0)->text() + ".ini");
        if(targetConfigFile.exists()){
            targetConfigFile.remove();
        }
        QFile targetHandleFile(QApplication::applicationDirPath() + "/handle/" + selectedItem.at(0)->text());
        if(targetHandleFile.exists()){
            targetHandleFile.remove();
        }

        if(configFile.copy(QApplication::applicationDirPath() + "/config/" + selectedItem.at(0)->text() + ".ini") &&
                handleFile.copy(QApplication::applicationDirPath() + "/handle/" + selectedItem.at(0)->text())){
            qDebug() << "files copied.";
            if(!handleFile.setPermissions(QFile::ExeGroup | QFile::ExeUser | QFile::ExeOwner)){
                QString comand = "sudo chmod +x " + QApplication::applicationDirPath() + "/handle/*";
                system(comand.toStdString().data());
            }
            qDebug() << "Have moved config and handle files.";

            QMessageBox message;
            message.setIcon(QMessageBox::Information);
            message.setText("System have copied target files.");
            QPushButton button;
            button.setText("Done");
            button.setBaseSize(QSize(60,40));
            message.addButton(&button,QMessageBox::AcceptRole);
            message.setDefaultButton(&button);
            message.setWindowFlags(Qt::WindowStaysOnTopHint);
            message.exec();
            connect(&button,SIGNAL(clicked(bool)),&message,SLOT(close()));
        }else{
            qDebug() << "copy files failed";
            QMessageBox message;
            message.setIcon(QMessageBox::Question);
            message.setText("System fail to copy target files.");
            QPushButton button;
            button.setText("OK");
            button.setBaseSize(QSize(60,40));
            message.addButton(&button,QMessageBox::AcceptRole);
            message.setDefaultButton(&button);
            message.setWindowFlags(Qt::WindowStaysOnTopHint);
            message.exec();
            connect(&button,SIGNAL(clicked(bool)),&message,SLOT(accept()));
        }
    }
}

void ImportMode::rejectPressed(){
    qDebug() << "Cancel Button pressed in ImportMode UI.";
    QDialog::close();
}

//读取U盘中存在的配置文件以及处理文件
void ModeCheck::run(){
    QDir USBDir("/media/pi/Camera");		//USB可挂载设备的母文件夹
    if(!USBDir.exists()){
        qDebug() << "Errors about the USB FileSystem";
    }else{
        if(USBDir.exists()){
            qDebug() << "USB device read OK.";// USBDir;

            QString temp = USBDir.absolutePath();
            QDir ConfigDir = QDir(temp.append("/config/"));				//进入到config文件夹，准备读取配置文件

            if(!ConfigDir.exists()){
                qDebug() << ConfigDir;
                qDebug() << "Could not find config folder.";	//没有找到配置文件
            }else{
                QStringList filter;		//过滤.ini类型的文件
                filter << "*.ini";
                ConfigDir.setNameFilters(filter);

                QFileInfoList list = ConfigDir.entryInfoList();		//得到过滤结果

                if(list.count() == 0){		//没有读到.ini文件
                    qDebug() << "Cannot find any .ini file.";
                }else{
                    qDebug() << "Found .ini file.";

                    for(int i = list.count() - 1 ; i >= 0  ; i--){
                        readConfigName.insert(list.at(i).completeBaseName(),list.at(i).absoluteFilePath());		//只读取文件名，不读配置名
                        qDebug() << list.at(i).completeBaseName() << " : " << readConfigName.value(list.at(i).completeBaseName());
                    }

                    temp = USBDir.absolutePath();
                    QDir HandleDir = QDir(temp.append("/handle/"));			//进入handle文件夹，准备读取处理程序

                    if(!HandleDir.exists()){
                        qDebug() << HandleDir;
                        qDebug() << "Could not find handle folder.";		//未找到handle文件夹
                    }else{
                        HandleDir.setFilter(QDir::NoSymLinks | QDir::Files);			//过滤，只保留文件，去除符号链接文件
                        HandleDir.setSorting(QDir::Name);

                        list.clear();
                        list = HandleDir.entryInfoList();

                        if(list.count() == 0){
                            qDebug() << "Cannot find any executable file.";
                        }else{
                            qDebug() << "Found Executable file.";

                            for(int i = list.count() - 1 ; i >= 0 ; i--){
                                readHandleName.insert(list.at(i).completeBaseName(),list.at(i).absoluteFilePath());		//读取处理文件，包含拓展格式
                                qDebug() << list.at(i).completeBaseName() << " : " << readHandleName.value(list.at(i).completeBaseName());
                            }

                            qDebug() << "Config and Handle files read done";
                        }
                    }
                }
            }

        }else{
            qDebug() << "USB device is not reachable.";
        }
    }
}
