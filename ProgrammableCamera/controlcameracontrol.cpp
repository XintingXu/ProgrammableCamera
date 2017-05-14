#include <controlcameracontrol.h>
#include <QFile>
#include <QFileInfo>

ControlCameraControl::ControlCameraControl():QThread(){
    this->settingFile = NULL;
    QFileInfo *cameraFile;
    for(int i = 3 ; i >= 0 ; i--){
        cameraFile = new QFileInfo(cameraPath[i]);
        if(cameraFile == NULL || !cameraFile->isSymLink()){
            qDebug() << "Internel Error : read camera file not SymLink.";
        }else{
            QString target = cameraFile->symLinkTarget();
            mapOfCameraNumber.insert(QString(i),target.right(1).toInt());
        }
        delete cameraFile;
        cameraFile = NULL;
    }

    for(int i = 3 ; i >= 0 ; i--){
        camera[i] = new CameraControl(mapOfCameraNumber[QString(i)]);
    }
    qDebug() << "ControlCameraControl init done";
}

ControlCameraControl::~ControlCameraControl(){
    ;
}

void ControlCameraControl::run(){
    qDebug() << "ControlCameraControl :" << " run() threadID :" << this->currentThreadId();
    startCapture();
    for(int i = 0 ; i < 4 ; i ++){
        camera[i]->wait();
    }
    captureDoneDeal();

    emit captureDone(&captureDoneImage);
}

bool ControlCameraControl::setConfigFile(QString fileName){
    qDebug() << "ControlCameraControl :" << " setConfigFile() threadID :" << this->currentThreadId();
    qDebug() << "configFilePath : " << fileName;

    for(int i = 0 ; i < 4 ; i ++){
        controlValue[i].clear();
        camera[i]->CameraConfigure.clear();
        camera[i]->captured.clear();
    }

    settingFile = new QSettings(fileName,QSettings::IniFormat);
    settingFile->setParent(this);
    settingFile->beginGroup("About");

    if(!settingFile->value("NAME").isNull()){
        QString info = QString("read setting :") + QString(settingFile->value("NAME").toString());
        emit logText(info);
        qDebug() << "read setting :" << settingFile->value("NAME").toString();
        qDebug() << "setting DESCRIBE :" << settingFile->value("DESCRIBE").toString();
        qDebug() << "setting AUTHOR :" << settingFile->value("AUTHOR").toString();
        qDebug() << "setting DATE :" << settingFile->value("DATE").toString();
        qDebug() << "setting TOTALCAPTURE :" << settingFile->value("TOTALCAPTURE").toString();

        if(settingFile->value("TOTALCAPTURE").toInt() == 0){
            qDebug() << "setting File defines 0 capture.";
            return false;
        }else{
            int totalCapture = settingFile->value("TOTALCAPTURE").toInt();
            settingFile->endGroup();
            for(int i = 0 ; i < totalCapture ; i++){
                settingFile->beginGroup("CAPTURE" + QString::number(i));
                QStringList allKeys = settingFile->allKeys();
                if(allKeys.length() == 0){
                    qDebug() << "allKeys.length() == 0";
                }else{
                    CameraControlValue *value = new CameraControlValue;
                    int cameraID = settingFile->value("CAMERA").toInt();
                    for(int j = 0 ; j < allKeys.length() ; j++){
                        QString Key = allKeys.at(j);
                        QString Value = settingFile->value(allKeys.at(j)).toString();
                        setCameraControlValue(value,&Key,&Value);
                    }
                    controlValue[cameraID].push_back(*value);
                    delete value;
                }
                settingFile->endGroup();
            }
            qDebug() << "settinf files read Done.";
            for(int i = 0 ; i < 4 ; i++){
                qDebug() << "camera " << i << "get :" << controlValue[i].length() << " configs";
            }

            return true;
        }
    }
    return false;
}

void ControlCameraControl::captureDoneDeal(){
    threadLock.lock();

    qDebug() << "ControlCameraControl :" << " captureDoneDeal() threadID :" << this->currentThreadId();

    for(int i = 0 ; i < 4 ; i++){
        if(camera[i]->captured.length() != 0){
            qDebug() << "captured :" << camera[i]->captured.length() << " in Camera : " << i;
            for(int j = 0 ; j < camera[i]->captured.length() ; j++){
                captureDoneImage.push_back(camera[i]->captured.at(j));
            }
        }
    }

    this->isCapturing = false;
    for(int i = 0 ; i < 4 ; i++){
        controlValue[i].clear();
    }

    threadLock.unlock();
}

void ControlCameraControl::startCapture(){
    threadLock.lock();

    qDebug() << "ControlCameraControl :" << " startCapture() threadID :" << this->currentThreadId();

    this->isCapturing = true;
    for(int i = 0 ; i < 4 ; i ++){
        if(controlValue[i].length() != 0){
            camera[i]->CameraConfigure = controlValue[i];
            qDebug() << "camera " << i << " load config";
            camera[i]->isUsed = true;
            camera[i]->start(QThread::TimeCriticalPriority);
        }else{
            camera[i]->isUsed = false;
        }
    }

    threadLock.unlock();
}

//void ControlCameraControl::updateUICamera(cv::Mat *img, int cameraNumber){
//    emit updateUI(img,cameraNumber);
//}

void ControlCameraControl::setCameraControlValue(CameraControlValue *control, QString *Key, QString *Value){
    /*cv::Size2i Size;	//照片的分辨率
    int Brightness;		//拍照时的亮度
    int Contrast;		//拍照时的对比度
    int Saturation;		//拍照时的色饱和度
    int Hue;			//拍照时的色调
    int Resolution;		//拍照时的清晰度
    double Gamma;		//拍照时的Gamma值
    int WhiteBalance;	//拍照时的白平衡
    int Exposure;*/		//拍照时的曝光度

    if(*Key == "HEIGHT"){
        control->Size.height = Value->toInt();
        return ;
    }
    if(*Key == "WIDTH"){
        control->Size.width = Value->toInt();
        return ;
    }
    if(*Key == "BRIGHTNESS"){
        control->Brightness = Value->toInt();
        return ;
    }
    if(*Key == "CONTRAST"){
        control->Contrast = Value->toInt();
        return ;
    }
    if(*Key == "SATURATION"){
        control->Saturation = Value->toInt();
        return ;
    }
    if(*Key == "Hue"){
        control->Hue = Value->toInt();
        return ;
    }
    if(*Key == "RESOLUTION"){
        control->Resolution = Value->toInt();
        return ;
    }
    if(*Key == "GAMMA"){
        control->Gamma = Value->toDouble();
        return ;
    }
    if(*Key == "WHITEBALANCE"){
        control->WhiteBalance = Value->toInt();
        return ;
    }
    if(*Key == "EXPOSURE"){
        control->Exposure = Value->toInt();
        return ;
    }
}
