#include "CameraControl.h"

/*
CameraControl.h的函数实现

作者：徐欣廷
创建时间：2017-02-22
修改时间：2017-02-25
*/

extern int (*myclose)(int);

//CameraControl的构造函数，需要传入摄像头的编号作为参数，默认设为0
CameraControl::CameraControl(int CameraNumber = 0){
    this->CameraNumber = CameraNumber;
    this->CameraCapture = cvCaptureFromCAM(this->CameraNumber);

    this->CameraConfigure.Size.height = 720;
    this->CameraConfigure.Size.width = 1280;
    this->CameraConfigure.Brightness = 0;
    this->CameraConfigure.Contrast = 0;
    this->CameraConfigure.Saturation = 0;
    this->CameraConfigure.Tone = 0;
    this->CameraConfigure.Resolution = 0;
    this->CameraConfigure.Gamma = 0.0;
    this->CameraConfigure.WhiteBalance = 0;
    this->CameraConfigure.Exposure = 0;

    CPU_ZERO(&setMask);
    CPU_SET(CameraNumber % 4, &setMask);

    if(pthread_setaffinity_np(pthread_self(),sizeof(setMask),&setMask) < 0){
        qDebug() << "set thread to specific cpu core failed";
    }
}

//CameraControl的析构函数，释放内存
CameraControl::~CameraControl(){
    cvReleaseCapture(&CameraCapture);
    pthread_exit(NULL);
    qDebug("CameraControl of Camera : %d is deleted.",this->CameraNumber);
}

//设定摄像头的编号，返回值为bool
bool CameraControl::setCamera(int CameraNumber = 0){
    cvReleaseCapture(&CameraCapture);
    CameraCapture = cvCaptureFromCAM(CameraNumber);
    if(CameraCapture != NULL){
        this->CameraNumber = CameraNumber;
        qDebug() << "Opened Camera : " << this->CameraNumber << " ." << endl;
        return true;
    }else{
        this->CameraNumber = 0;
        CameraCapture = cvCaptureFromCAM(0);
        if(CameraCapture != NULL){
            qDebug() << "Opened the default camera 0." << endl;
            return true;
        }else{
            qDebug() << "Cannot open default camera 0." << endl;
            return false;
        }
    }
    return true;
}

//设定拍照时的亮度，返回值为bool
bool CameraControl::setBrightness(int Brightness = 0){
    if(Brightness != 0){
        if(cvSetCaptureProperty(CameraCapture,CV_CAP_PROP_BRIGHTNESS,(double)Brightness) == 0){
            this->CameraConfigure.Brightness = Brightness;
            qDebug() << "CameraCapture " << this->CameraNumber << " set Brightness to :" << Brightness << " successfully." << endl;
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set Brightness failed." << endl;
            return false;
        }
    }else{  //let the camera set the brightness automatically
        return true;
    }
}

//设定拍照时的对比度，返回值为bool
bool CameraControl::setContrast(int Contrast = 0){
    if(Contrast != 0){
        if(cvSetCaptureProperty(CameraCapture,CV_CAP_PROP_CONTRAST,(double)Contrast) == 0){
            this->CameraConfigure.Contrast = Contrast;
            qDebug() << "CameraCapture " << this->CameraNumber << " set Contrast to :" << Contrast << " successfully." << endl;
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set Contrast failed." << endl;
            return false;
        }
    }else{  //let the camera set the Contrast automatically
        return true;
    }
}

//设定拍照时的色饱和度，返回值为bool
bool CameraControl::setSaturation(int Saturation = 0){
    if(Saturation != 0){
        if(cvSetCaptureProperty(CameraCapture,CV_CAP_PROP_SATURATION,(double)Saturation) == 0){
            this->CameraConfigure.Saturation = Saturation;
            qDebug() << "CameraCapture " << this->CameraNumber << " set Saturation to :" << Saturation << " successfully." << endl;
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set Saturation failed." << endl;
            return false;
        }
    }else{  //let the camera set the Saturation automatically
        return true;
    }
}

//设定拍照时的色调，返回值为bool
bool CameraControl::setTone(int Tone = 0){
    if(Tone != 0){
        if(cvSetCaptureProperty(CameraCapture,CV_CAP_PROP_HUE,(double)Tone) == 0){
            this->CameraConfigure.Tone = Tone;
            qDebug() << "CameraCapture " << this->CameraNumber << " set Tone to :" << Tone << " successfully." << endl;
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set Tone failed." << endl;
            return false;
        }
    }else{  //let the camera set the Tone automatically
        return true;
    }
}

//设定拍照时的清晰度，返回值为bool
bool CameraControl::setResolution(int Resolution = 0){  //This function is left to be compeleted with the camera API.
    this->CameraConfigure.Resolution = Resolution;
    return true;
}

//设定拍照时的Gamma值，返回值为bool
bool CameraControl::setGamma(double Gamma = 0){
    if(Gamma != 0){
        if(cvSetCaptureProperty(CameraCapture,CV_CAP_PROP_GAMMA,(double)Gamma) == 0){
            this->CameraConfigure.Gamma = Gamma;
            qDebug() << "CameraCapture " << this->CameraNumber << " set Gamma to :" << Gamma << " successfully." << endl;
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set Gamma failed." << endl;
            return false;
        }
    }else{  //let the camera set the Gamma automatically
        return true;
    }
}

//设定拍照时的白平衡，返回值为bool
bool CameraControl::setWhiteBalance(int WhiteBalance = 0){
    if(WhiteBalance != 0){
        if(cvSetCaptureProperty(CameraCapture,CV_CAP_PROP_WHITE_BALANCE_V,(double)WhiteBalance) == 0){
            this->CameraConfigure.WhiteBalance = WhiteBalance;
            qDebug() << "CameraCapture " << this->CameraNumber << " set WhiteBalance to :" << WhiteBalance << " successfully." << endl;
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set WhiteBalance failed." << endl;
            return false;
        }
    }else{  //let the camera set the WhiteBalance automatically
        return true;
    }
}

//设定拍照时的曝光度，返回值为bool
bool CameraControl::setExposure(int Exposure = 0){
    if(Exposure != 0){
        if(cvSetCaptureProperty(CameraCapture,CV_CAP_PROP_EXPOSURE,(double)Exposure) == 0){
            this->CameraConfigure.Exposure = Exposure;
            qDebug() << "CameraCapture " << this->CameraNumber << " set Exposure to :" << Exposure << " successfully." << endl;
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set Exposure failed." << endl;
            return false;
        }
    }else{  //let the camera set the Exposure automatically
        return true;
    }
}

//设定拍照时的分辨率，返回值为bool
bool CameraControl::setHightAndWidth(cv::Size2i Size = cv::Size2i(720,1280)){
    if((cvSetCaptureProperty(CameraCapture,CV_CAP_PROP_FRAME_HEIGHT,Size.height) == 0) &&
        (cvSetCaptureProperty(CameraCapture,CV_CAP_PROP_FRAME_WIDTH,Size.width) == 0)){
        this->CameraConfigure.Size = Size;
        qDebug() << "Frame size set OK." << endl;
        return true;
    }
    else{
        this->CameraConfigure.Size.height = 720;
        this->CameraConfigure.Size.width = 1280;
        qDebug() << "Frame size set Error." << endl;
        return false;
    }
}


void CameraControl::run(){
    if(this->isUsed){
        setBrightness(32);
        setContrast(32);
        setExposure(32);
        setGamma(32);
        setHightAndWidth();
        setWhiteBalance(32);
    }else{
        ;
    }
}
