#include <programmablecamera.h>
#include <QProcess>
#include <QDateTime>
#include <QFile>
#include <QFileInfoList>
#include <QDir>
#include <QApplication>
#include <QStringList>

HandleControl::HandleControl(){
    ;
}

HandleControl::~HandleControl(){
    ;
}

void HandleControl::run(){
    QProcess::execute(currentHandlePath);

    emit handleDone();
}

void HandleControl::setCurrentHandlePath(QString path){
    currentHandlePath = path;
}

SaveControl::SaveControl(){
    mapOfMode.insert("Save All",0);
    mapOfMode.insert("Save Result",1);
}

SaveControl::~SaveControl(){
    ;
}

void SaveControl::run(){
    QDateTime currentDateTime;
    currentDateTime = QDateTime::currentDateTime();
    if(this->saveMode == 0){
        QDir temp_images_Dir(QApplication::applicationDirPath() + "/temp_images/");
        QDir images_Dir(QApplication::applicationDirPath() + "/images/");

        temp_images_Dir.setFilter(QDir::NoSymLinks | QDir::Files);
        images_Dir.setFilter(QDir::NoSymLinks | QDir::Files);

        QFileInfoList list = images_Dir.entryInfoList();

        for(int  i = 0 ; i < list.length() ; i++){
            if(list.at(i).baseName().toLower().contains(handleName,Qt::CaseInsensitive)){
                QString fileName;
                QString command;
                fileName.sprintf("%s-%s.jpg",currentDateTime.toString("yyyy-MM-dd-HH-mm-ss").toStdString().data(),list.at(i).baseName().toStdString().data());
                command.sprintf("sudo cp %s /media/pi/Camera/images/%s",list.at(i).absoluteFilePath().toStdString().data(),fileName.toStdString().data());

                system(command.toStdString().data());
            }
        }

        list.clear();
        list = temp_images_Dir.entryInfoList();

        for(int  i = 0 ; i < list.length() ; i++){
            if(list.at(i).baseName().toLower().contains(handleName,Qt::CaseInsensitive)){
                QString fileName;
                QString command;
                fileName.sprintf("%s-%s_temp.jpg",currentDateTime.toString("yyyy-MM-dd-HH-mm-ss").toStdString().data(),list.at(i).baseName().toStdString().data());
                command.sprintf("sudo cp %s /media/pi/Camera/images/%s",list.at(i).absoluteFilePath().toStdString().data(),fileName.toStdString().data());

                system(command.toStdString().data());
            }
        }
    }else{
        QDir images_Dir(QApplication::applicationDirPath() + "/images/");

        images_Dir.setFilter(QDir::NoSymLinks | QDir::Files);

        QFileInfoList list = images_Dir.entryInfoList();

        for(int  i = 0 ; i < list.length() ; i++){
            if(list.at(i).baseName().toLower().contains(handleName,Qt::CaseInsensitive)){
                QString fileName;
                QString command;
                fileName.sprintf("%s-%s.jpg",list.at(i).baseName().toStdString().data(),currentDateTime.toString("yyyy-MM-dd-HH-mm-ss").toStdString().data());
                command.sprintf("sudo mv %s /media/Camera/images/%s",list.at(i).absoluteFilePath().toStdString().data(),fileName.toStdString().data());

                system(command.toStdString().data());
            }
        }
    }

    emit saveDone();
}

void SaveControl::setSaveMode(QString name){
    this->saveMode = mapOfMode[name];
}

void SaveControl::setHandleName(QString name){
    this->handleName = name;
}
