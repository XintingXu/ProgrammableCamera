#include "CameraControl.h"
#include <QTimer>

/*
CameraControl.h的函数实现

作者：徐欣廷
创建时间：2017-02-22
修改时间：2017-02-25
*/

extern int (*myclose)(int);

//CameraControl的构造函数，需要传入摄像头的编号作为参数，默认设为0
CameraControl::CameraControl(int CameraNumber = 0){
    this->fd = 0;

    this->CameraNumber = CameraNumber;
    //this->CameraCapture = cvCaptureFromCAM(this->CameraNumber);

    CPU_ZERO(&setMask);
    CPU_SET(CameraNumber % 4, &setMask);

    if(pthread_setaffinity_np(pthread_self(),sizeof(setMask),&setMask) < 0){
        qDebug() << "set thread to specific cpu core failed";
    }
    qDebug() << "Thread CameraControl :" << CameraNumber << " created.";

    //startTimer(100,Qt::PreciseTimer);   //start timer of viewfinder update

    //    if(this->CameraNumber == 0){
    //        this->QsemTimeout = new QSemaphore(0);
    //        this->getImageShourtCut = new getShortCut(this->CameraNumber);
    //        connect(getImageShourtCut,SIGNAL(updateUI(cv::Mat*,int)),this,SLOT(updateUICamera(cv::Mat*,int)),Qt::QueuedConnection);
    //        this->getImageShourtCut->isExiting = false;
    //        this->getImageShourtCut->timeout = this->QsemTimeout;
    //        update.setTimerType(Qt::PreciseTimer);
    //        this->getImageShourtCut->start(QThread::HighPriority);
    //        update.start(100);
    //        connect(&update,SIGNAL(timeout()),this,SLOT(onTimerUpdate()),Qt::DirectConnection);
    //        qDebug() << "camera :" << this->CameraNumber << " create timer update.";
    //    }else{
    //        ;
    //    }
}

//CameraControl的析构函数，释放内存
CameraControl::~CameraControl(){
    //pthread_exit(NULL);
    qDebug("CameraControl of Camera : %d is deleted.",this->CameraNumber);
}

//get CameraNumber of this
int CameraControl::getCameraNumber(){
    return this->CameraNumber;
}

//设定摄像头的编号，返回值为bool
bool CameraControl::setCamera(int CameraNumber = 0){
//    cvReleaseCapture(&CameraCapture);
    CameraCapture.open(CameraNumber);
    if(CameraCapture.isOpened()){
        this->CameraNumber = CameraNumber;
        qDebug() << "Opened Camera : " << this->CameraNumber << " .";
        CameraCapture.release();
        return true;
    }else{
        this->CameraNumber = 0;
        CameraCapture.open(0);
        if(CameraCapture.isOpened()){
            qDebug() << "Opened the default camera 0.";
            CameraCapture.release();
            return true;
        }else{
            qDebug() << "Cannot open default camera 0.";
            CameraCapture.release();
            return false;
        }
    }
    CameraCapture.release();
    return true;
}

//设定拍照时的亮度，返回值为bool
bool CameraControl::setBrightness(int Brightness = -8193){
    if(Brightness != -8193){
        if(Brightness > 64){
            Brightness = 64;
        }
        if(Brightness < -64){
            Brightness = -64;
        }
        struct v4l2_control control_brightness;
        control_brightness.id = V4L2_CID_BRIGHTNESS;
        control_brightness.value = Brightness;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_brightness) == 0){
            qDebug() << "CameraCapture " << this->CameraNumber << " set Brightness to :" << Brightness << " successfully.";
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set Brightness failed.";
            return false;
        }
    }else{  //let the camera set the brightness automatically
        struct v4l2_control control_brightness;
        control_brightness.id = V4L2_CID_BRIGHTNESS;
        control_brightness.value = -8193;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_brightness) == 0){
            return true;
        }else{
            return false;
        }
    }
}

//设定拍照时的对比度，返回值为bool
bool CameraControl::setContrast(int Contrast = 57343){
    if(Contrast != 57343){
        if(Contrast > 64){
            Contrast = 64;
        }
        if(Contrast < 0){
            Contrast = 0;
        }

        struct v4l2_control control_contrast;
        control_contrast.id = V4L2_CID_CONTRAST;
        control_contrast.value = Contrast;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_contrast) == 0){
            qDebug() << "CameraCapture " << this->CameraNumber << " set Contrast to :" << Contrast << " successfully.";
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set Contrast failed.";
            return false;
        }
    }else{  //let the camera set the Contrast automatically
        struct v4l2_control control_contrast;
        control_contrast.id = V4L2_CID_CONTRAST;
        control_contrast.value = 57343;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_contrast) == 0){
            return true;
        }else{
            return true;
        }
    }
}

//设定拍照时的色饱和度，返回值为bool
bool CameraControl::setSaturation(int Saturation = 57343){
    if(Saturation != 57343){
        if(Saturation > 128){
            Saturation = 128;
        }
        if(Saturation < 0){
            Saturation = 0;
        }

        struct v4l2_control control_saturation;
        control_saturation.id = V4L2_CID_SATURATION;
        control_saturation.value = Saturation;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_saturation) == 0){
            qDebug() << "CameraCapture " << this->CameraNumber << " set Saturation to :" << Saturation << " successfully.";
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set Saturation failed.";
            return false;
        }
    }else{  //let the camera set the Saturation automatically
        struct v4l2_control control_saturation;
        control_saturation.id = V4L2_CID_SATURATION;
        control_saturation.value = 57343;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_saturation) == 0){
            return true;
        }else{
            return false;
        }
    }
}

//设定拍照时的色调(hue)，返回值为bool
bool CameraControl::setHue(int Hue = -8193){
    if(Hue != -8193){
        if(Hue > 40){
            Hue = 40;
        }
        if(Hue < -40){
            Hue = -40;
        }

        struct v4l2_control control_Hue;
        control_Hue.id = V4L2_CID_HUE;
        control_Hue.value = Hue;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_Hue) == 0){
            qDebug() << "CameraCapture " << this->CameraNumber << " set Hue to :" << Hue << " successfully.";
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set Hue failed.";
            return false;
        }
    }else{  //let the camera set the Hue automatically
        struct v4l2_control control_Hue;
        control_Hue.id = V4L2_CID_HUE;
        control_Hue.value = -8193;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_Hue) == 0){
            return true;
        }else{
            return false;
        }
    }
}

//设定拍照时的清晰度(sharpness)，返回值为bool
bool CameraControl::setResolution(int Resolution = 57343){  //This function is left to be compeleted with the camera API.
    if(Resolution != 57343){
        if(Resolution > 6){
            Resolution = 6;
        }
        if(Resolution < 0){
            Resolution = 0;
        }

        struct v4l2_control control_resolution;
        control_resolution.id = V4L2_CID_SHARPNESS;
        control_resolution.value = Resolution;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_resolution) == 0){
            qDebug() << "CameraCapture " << this->CameraNumber << " set Resolution to :" << Resolution << " successfully.";
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set Resolution failed.";
            return false;
        }
    }else{  //let the camera set the Resolution automatically
        struct v4l2_control control_resolution;
        control_resolution.id = V4L2_CID_SHARPNESS;
        control_resolution.value = 57343;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_resolution) == 0){
            return true;
        }else{
            return false;
        }
    }
}

//设定拍照时的Gamma值，返回值为bool
bool CameraControl::setGamma(int Gamma = 57343){
    if(Gamma != 57343){
        if(Gamma > 500){
            Gamma = 500;
        }
        if(Gamma < 72){
            Gamma = 72;
        }
        struct v4l2_control control_gamma;
        control_gamma.id = V4L2_CID_GAMMA;
        control_gamma.value = Gamma;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_gamma) == 0){
            qDebug() << "CameraCapture " << this->CameraNumber << " set Gamma to :" << Gamma << " successfully.";
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set Gamma failed.";
            return false;
        }
    }else{  //let the camera set the Gamma automatically
        struct v4l2_control control_gamma;
        control_gamma.id = V4L2_CID_GAMMA;
        control_gamma.value = 57343;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_gamma) == 0){
            return true;
        }else{
            return false;
        }
    }
}

//设定拍照时的白平衡，返回值为bool
bool CameraControl::setWhiteBalance(int WhiteBalance = 0){
    if(WhiteBalance != 0){
        if(WhiteBalance > 6500){
            WhiteBalance = 6500;
        }
        if(WhiteBalance < 2800){
            WhiteBalance = 2800;
        }
        struct v4l2_control control_whitebalance;
        control_whitebalance.id = V4L2_CID_AUTO_WHITE_BALANCE;
        control_whitebalance.value = 0;
        ioctl(fd,VIDIOC_S_CTRL,&control_whitebalance);
        control_whitebalance.id = V4L2_CID_WHITE_BALANCE_TEMPERATURE;
        control_whitebalance.value = WhiteBalance;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_whitebalance) == 0){
            qDebug() << "CameraCapture " << this->CameraNumber << " set WhiteBalance to :" << WhiteBalance << " successfully.";
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set WhiteBalance failed.";
            return false;
        }
    }else{  //let the camera set the WhiteBalance automatically
        struct v4l2_control control_whitebalance;
        control_whitebalance.id = V4L2_CID_AUTO_WHITE_BALANCE;
        control_whitebalance.value = 1;
        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_whitebalance) == 0){
            return true;
        }else{
            return false;
        }
    }
}

//设定拍照时的曝光度，返回值为bool
bool CameraControl::setExposure(int Exposure = 0){
    if(Exposure != 0){
        if(Exposure > 5000){
            Exposure = 5000;
        }
        if(Exposure < 1){
            Exposure = 1;
        }

        struct v4l2_control control_exposure;
        control_exposure.id = V4L2_CID_EXPOSURE_AUTO;
        control_exposure.value = V4L2_EXPOSURE_MANUAL;
        ioctl(fd,VIDIOC_S_CTRL,&control_exposure);
        control_exposure.id = V4L2_CID_EXPOSURE_ABSOLUTE;
        control_exposure.value = Exposure;

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_exposure) == 0){
            qDebug() << "CameraCapture " << this->CameraNumber << " set Exposure to :" << Exposure << " successfully.";
            return true;
        }else{
            qDebug() << "CameraCapture " << this->CameraNumber << " set Exposure failed.";
            return false;
        }
    }else{  //let the camera set the Exposure automatically
        struct v4l2_control control_exposure;
        control_exposure.id = V4L2_CID_EXPOSURE_AUTO;
        control_exposure.value = V4L2_EXPOSURE_APERTURE_PRIORITY;
        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_exposure) == 0){
            return true;
        }else{
            return false;
        }
    }
}

//设定拍照时的分辨率，返回值为bool
//unused
bool CameraControl::setHightAndWidth(cv::Size2i Size = cv::Size2i(720,1280)){
//    this->CameraCapture = new cv::VideoCapture(this->CameraNumber);
//    if((this->CameraCapture->set(CV_CAP_PROP_FRAME_HEIGHT,Size.height) == 0) &&
//            (this->CameraCapture->set(CV_CAP_PROP_FRAME_WIDTH,Size.width) == 0)){
//        this->CameraConfigure.Size = Size;
//        qDebug() << "Frame size set OK.";
//        return true;
//    }
//    else{
//        this->CameraConfigure.Size.height = 720;
//        this->CameraConfigure.Size.width = 1280;
//        qDebug() << "Frame size set Error.";
//        return false;
//    }
//    this->CameraCapture->release();
    Size = Size;
    return true;
}


void CameraControl::run(){
    qDebug() << "camera :" << this->CameraNumber << " run() threadID :" << this->currentThreadId();

    if(this->isUsed){
        QString fileName = "/dev/video";
        fileName = fileName + QString::number(this->CameraNumber);

        for(int count = 0 ; count < this->CameraConfigure.length() ;){
            fd = open(fileName.toStdString().data(),O_RDWR);
            if(fd == -1){
                qDebug() << "Cannot Open Camera :" << this->CameraNumber;
            }else{
                setBrightness(this->CameraConfigure.at(count).Brightness);
                setContrast(this->CameraConfigure.at(count).Contrast);
                setExposure(this->CameraConfigure.at(count).Exposure);
                setGamma(this->CameraConfigure.at(count).Gamma);
                setWhiteBalance(this->CameraConfigure.at(count).WhiteBalance);
                setSaturation(this->CameraConfigure.at(count).Saturation);
                setHue(this->CameraConfigure.at(count).Hue);

                struct v4l2_control control_quality;
                control_quality.id = V4L2_CID_JPEG_COMPRESSION_QUALITY;
                control_quality.value = 100;
                if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_quality)){
                    qDebug() << "Set Camera to be best quality.";
                }

                if(myclose(fd) == 0){
                    qDebug() << "Camera :" << this->CameraNumber << " closed";
                    fd = 0;
                }else{
                    qDebug() << "Cannot close Camera :" << this->CameraNumber << " Please check.";
                    myclose(fd);
                    fd = 0;
                }

                this->CameraCapture.open(this->CameraNumber);
                qDebug() << "get size " << this->CameraConfigure.at(count).Size.width << "x" << this->CameraConfigure.at(count).Size.height;
                this->CameraCapture.set(CV_CAP_PROP_FRAME_HEIGHT,this->CameraConfigure.at(count).Size.height);
                this->CameraCapture.set(CV_CAP_PROP_FRAME_WIDTH,this->CameraConfigure.at(count).Size.width);
                this->CameraCapture.set(CV_CAP_PROP_FPS,30);

                if(this->CameraCapture.isOpened()){
                    this->CameraCapture.grab();
                    cv::Mat captureTemp;
                    if(this->CameraCapture.read(captureTemp)){
                        captured.push_back(captureTemp);
                        this->CameraCapture.release();

                        qDebug() << "CameraControl : run() : CameraCapture :" << &this->CameraCapture;

                        qDebug() << "Quered one frame from :" << this->CameraNumber << captureTemp.cols << "x" << captureTemp.rows;

                        count++;

                    }else{
                        qDebug() << "Cannot grab from Camera :" << this->CameraNumber;
                    }

                }else{
                    this->CameraCapture.release();
                    QThread::msleep(30);
                }

                this->CameraCapture.release();
            }
        }

        this->CameraCapture.release();
    }else{
        qDebug() << "camera :" << this->CameraNumber << " isn't used";
    }
}




getShortCut::getShortCut(int CameraNumber){
    this->CameraNUmber = CameraNumber;
}

getShortCut::~getShortCut(){
    ;
}

void getShortCut::run(){
    //qDebug() << "camera :" << this->CameraNUmber << " onTimerUpdate() threadID :" << this->currentThreadId();

    cv::VideoCapture CameraCapture(this->CameraNUmber);

    if(!CameraCapture.isOpened()){
        qDebug() << "VideoFinder cannnot open :" << this->CameraNUmber;
    }

    if(CameraCapture.isOpened()){
        CameraCapture.set(CV_CAP_PROP_FRAME_HEIGHT,360);
        CameraCapture.set(CV_CAP_PROP_FRAME_WIDTH,480);
        cv::Mat captured;
        CameraCapture.grab();

        if(CameraCapture.read(captured)){
            cv::cvtColor(captured,captured,CV_BGR2RGB);
            cv::resize(captured,captured,cv::Size(400,225),0,0,CV_INTER_NN);
            this->image = QImage((uchar*)captured.data,captured.cols,captured.rows,captured.step,QImage::Format_RGB888).copy();
        }

        CameraCapture.release();
        captured.release();

        //qDebug() << "resize width:" << (this->image).width() << " height:" << (this->image).height();

        emit updateUI((this->image).copy(),this->CameraNUmber);
    }else{
        qDebug() << "Cannot open Camera :" << this->CameraNUmber;
    }
}
