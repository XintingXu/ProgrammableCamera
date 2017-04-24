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

        if(cameraFile != NULL){
            delete cameraFile;
            cameraFile = NULL;
        }
    }

    for(int i = 3 ; i >= 0 ; i--){
        camera[i] = new CameraControl(mapOfCameraNumber[QString(i)]);
        connect(camera[i],SIGNAL(haveCaptured(CameraControl*)),this,SLOT(captureFinished(CameraControl*)));
        //        connect(camera[i],SIGNAL(updateUI(IplImage*,int)),this,SLOT(updateUICamera(IplImage*,int)),Qt::QueuedConnection);
    }
    qDebug() << "ControlCameraControl init done";
}

ControlCameraControl::~ControlCameraControl(){
    for(int i = 3 ; i >= 0 ; i--){
        delete camera[i];
        camera[i] = NULL;

        if(controlValue[i].length() != 0){
            while(controlValue[i].length() != 0){
                delete controlValue[i].first();
                controlValue[i].removeFirst();
            }
        }

        if(((QList<IplImage *>*) &captured[i])->length() != 0){
            while(((QList<IplImage *>*) &captured[i])->length() != 0){
                cvReleaseImage(&(((QList<IplImage *>*) &captured[i])->first()));
                ((QList<IplImage *>*) &captured[i])->removeFirst();
            }
        }
    }
}

void ControlCameraControl::run(){
    qDebug() << "ControlCameraControl :" << " run() threadID :" << this->currentThreadId();
}

bool ControlCameraControl::setConfigFile(QString *fileName){
    qDebug() << "ControlCameraControl :" << " setConfigFile() threadID :" << this->currentThreadId();

    settingFile = new QSettings(*fileName,QSettings::IniFormat);
    if(!settingFile->value("About").isNull()){
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
                return false;
                qDebug() << "setting File defines 0 capture.";
            }else{
                int totalCapture = settingFile->value("TOTALCAPTURE").toInt();
                settingFile->endGroup();
                for(int i = 0 ; i < totalCapture ; i++){
                    settingFile->beginGroup("CAPTURE" + QString::number(i + 1));
                    QStringList allKeys = settingFile->allKeys();
                    if(allKeys.length() == 0){
                        return false;
                    }else{
                        CameraControlValue *value;
                        value = new CameraControlValue;
                        int cameraID = settingFile->value("CAMERA").toInt();
                        for(int j = 0 ; j < allKeys.length() ; j++){
                            QString Key = allKeys.at(i);
                            QString Value = settingFile->value(allKeys.at(i)).toString();
                            setCameraControlValue(value,&Key,&Value);
                        }
                        controlValue[cameraID].push_back(value);
                    }
                }
                qDebug() << "settinf files read Done.";
            }
        }
    }else{
        qDebug() << "Cannot open .ini file in ControlCameraControl.";
        return false;
    }
    return false;
}

//when a camera has captured,reload the config and capture the next image.
void ControlCameraControl::captureFinished(CameraControl *camera){
    qDebug() << "ControlCameraControl :" << " captureFinished() threadID :" << this->currentThreadId();
    int i;
    bool haveLeft = false;
    for(i = 3 ; i >= 0 ; i--){
        if(this->camera[i] == camera){
            this->haveCapturedNumber[i]++;
            haveLeft = true;
            break;
        }
    }
    if(this->haveCapturedNumber[i] < controlValue[i].length()){
        loadCapture(camera,i);
    }
    if(!haveLeft){
        captureDoneDeal();
    }
}

void ControlCameraControl::captureDoneDeal(){
    qDebug() << "ControlCameraControl :" << " captureDoneDeal() threadID :" << this->currentThreadId();

    for(int i = 0 ; i < 4 ; i++){
        if(((QList<IplImage *>*) &captured[i])->length() != 0){
            for(int j = 0 ; j < ((QList<IplImage *>*) &captured[i])->length() ; j++){
                ((QList<IplImage *>*) &captureDoneImage)->push_back(((QList<IplImage *>*) &captured[i])->at(j));
            }
        }
    }
    this->isCapturing = false;
    emit captureDone((QList<IplImage *>*) &captureDoneImage);
}

void ControlCameraControl::loadCapture(CameraControl *camera,int i){
    camera->CameraConfigure = *this->controlValue[i].at(haveCapturedNumber[i]);
    camera->start(QThread::TimeCriticalPriority);
}

void ControlCameraControl::startCapture(){
    qDebug() << "ControlCameraControl :" << " startCapture() threadID :" << this->currentThreadId();

    this->isCapturing = true;
    for(int i = 0 ; i < 4 ; i ++){
        if(controlValue[i].length() != 0){
            camera[i]->CameraConfigure = *controlValue[i].at(haveCapturedNumber[i]);
            camera[i]->start(QThread::TimeCriticalPriority);
        }
    }
}

//void ControlCameraControl::updateUICamera(IplImage *img, int cameraNumber){
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
