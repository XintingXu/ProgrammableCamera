#include "importmode.h"
#include "ui_importmode.h"

#include <QDir>
#include <QDebug>

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

    QMap <QString,QString> ::iterator it;
    for(it = readConfigName.begin() ; it != readConfigName.end() ; it ++){
        if(readHandleName.find(it.key()) != readHandleName.end()){
            ui->listWidget->addItem(it.key());
        }
    }
}

ImportMode::~ImportMode()
{
    delete ui;
}


//读取U盘中存在的配置文件以及处理文件
void ModeCheck::run(){
    QDir USBDir("/media/pi/");		//USB可挂载设备的母文件夹
    if(!USBDir.exists()){
        qDebug() << "Errors about the USB FileSystem" << endl;
    }else{
        USBDir.setFilter(QDir::Dirs | QDir::NoSymLinks);		//过滤文件夹，不包括符号链接
        USBDir.setSorting(QDir::Size);							//排序

        QFileInfoList list = USBDir.entryInfoList();
/*
        for (int i = 0 ; i < list.size() ; i ++){
            qDebug() << "i = " << i << " : " << list.at(i).absoluteFilePath() << endl;
        }
*/
        QFileInfo fileInfo = list.at(0);						//读取USB盘名

        USBpath = fileInfo.absoluteFilePath();					//得到绝对路径
//        qDebug() << USBpath << endl;

        USBDir = QDir(USBpath);

        if(USBDir.exists()){
            qDebug() << "USB device read OK." << endl;// USBDir << endl;
            USBDeviceOK = true;

            QString temp = USBpath;
            USBDir = QDir(temp.append("/config/"));				//进入到config文件夹，准备读取配置文件

            if(!USBDir.exists()){
                qDebug() << USBDir << endl;
                qDebug() << "Could not find config folder." << endl;	//没有找到配置文件
            }else{
                QStringList filter;		//过滤.ini类型的文件
                filter << "*.ini";
                USBDir.setNameFilters(filter);

                list = USBDir.entryInfoList();		//得到过滤结果

                if(list.count() == 0){		//没有读到.ini文件
                    qDebug() << "Cannot find any .ini file." << endl;
                }else{
                    qDebug() << "Found .ini file." << endl;

                    for(int i = 0 ; i < list.count() ; i++){
                        readConfigName.insert(list.at(i).completeBaseName(),list.at(i).absoluteFilePath());		//只读取文件名，不读配置名
                        qDebug() << list.at(i).completeBaseName() << " : " << readConfigName.value(list.at(i).completeBaseName()) << endl;
                    }

                    temp = USBpath;
                    USBDir = QDir(temp.append("/handle/"));			//进入handle文件夹，准备读取处理程序

                    if(!USBDir.exists()){
                        qDebug() << USBDir << endl;
                        qDebug() << "Could not find handle folder." << endl;		//未找到handle文件夹
                    }else{
                        USBDir.setFilter(QDir::NoSymLinks | QDir::Files);			//过滤，只保留文件，去除符号链接文件
                        USBDir.setSorting(QDir::Name);

                        list = USBDir.entryInfoList();

                        if(list.count() == 0){
                            qDebug() << "Cannot find any executable file." << endl;
                        }else{
                            qDebug() << "Found Executable file." << endl;

                            for(int i = 0; i < list.count() ; i++){
                                readHandleName.insert(list.at(i).completeBaseName(),list.at(i).absoluteFilePath());		//读取处理文件，包含拓展格式
                                qDebug() << list.at(i).completeBaseName() << " : " << readHandleName.value(list.at(i).completeBaseName()) << endl;
                            }

                            qDebug() << "Config and Handle files read done" << endl;
                        }
                    }
                }
            }

        }else{
            qDebug() << "USB device is not reachable." << endl;
        }
    }
}
