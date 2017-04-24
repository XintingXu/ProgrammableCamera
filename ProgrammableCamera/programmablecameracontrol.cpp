#include <programmablecamera.h>
#include <QProcess>

HandleControl::HandleControl(){
    ;
}

HandleControl::~HandleControl(){
    ;
}

void HandleControl::run(){
    QProcess::execute(currentHandlePath);
}

void HandleControl::setCurrentHandlePath(QString path){
    currentHandlePath = path;
}

SaveControl::SaveControl(){
    mapOfMode.insert("Save All",1);
    mapOfMode.insert("Save Result",1);
}

SaveControl::~SaveControl(){
    ;
}

void SaveControl::run(){
    ;
}

void SaveControl::setSaveMode(QString name){
    this->saveMode = mapOfMode[name];
}
