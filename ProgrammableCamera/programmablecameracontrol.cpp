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
    if(this->saveMode == 0){
        ;
    }else{
        ;
    }

    emit saveDone();
}

void SaveControl::setSaveMode(QString name){
    this->saveMode = mapOfMode[name];
}
