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

    this->CameraConfigure.Size.height = 720;
    this->CameraConfigure.Size.width = 1280;
    this->CameraConfigure.Brightness = 0;
    this->CameraConfigure.Contrast = 0;
    this->CameraConfigure.Saturation = 0;
    this->CameraConfigure.Hue = 0;
    this->CameraConfigure.Resolution = 0;
    this->CameraConfigure.Gamma = 0.0;
    this->CameraConfigure.WhiteBalance = 0;
    this->CameraConfigure.Exposure = 0;

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
    //        connect(getImageShourtCut,SIGNAL(updateUI(IplImage*,int)),this,SLOT(updateUICamera(IplImage*,int)),Qt::QueuedConnection);
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
    cvReleaseCapture(&CameraCapture);
    pthread_exit(NULL);
    //    this->update.stop();
    //    this->getImageShourtCut->isExiting = true;
    //    this->getImageShourtCut->exit();

    if(QsemTimeout != NULL){
        delete QsemTimeout;
        QsemTimeout = NULL;
    }
    //    if(getImageShourtCut != NULL){
    //        delete getImageShourtCut;
    //        getImageShourtCut = NULL;
    //    }
    //    this->getImageShourtCut->wait();
    qDebug("CameraControl of Camera : %d is deleted.",this->CameraNumber);
}

//get CameraNumber of this
int CameraControl::getCameraNumber(){
    return this->CameraNumber;
}

//设定摄像头的编号，返回值为bool
bool CameraControl::setCamera(int CameraNumber = 0){
    cvReleaseCapture(&CameraCapture);
    CameraCapture = cvCaptureFromCAM(CameraNumber);
    if(CameraCapture != NULL){
        this->CameraNumber = CameraNumber;
        qDebug() << "Opened Camera : " << this->CameraNumber << " .";
        return true;
    }else{
        this->CameraNumber = 0;
        CameraCapture = cvCaptureFromCAM(0);
        if(CameraCapture != NULL){
            qDebug() << "Opened the default camera 0.";
            return true;
        }else{
            qDebug() << "Cannot open default camera 0.";
            return false;
        }
    }
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_brightness)){
            this->CameraConfigure.Brightness = Brightness;
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_brightness)){
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_contrast)){
            this->CameraConfigure.Contrast = Contrast;
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_contrast)){
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_saturation)){
            this->CameraConfigure.Saturation = Saturation;
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_saturation)){
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_Hue)){
            this->CameraConfigure.Hue = Hue;
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_Hue)){
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_resolution)){
            this->CameraConfigure.Resolution = Resolution;
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_resolution)){
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_gamma)){
            this->CameraConfigure.Gamma = Gamma;
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_gamma)){
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_whitebalance)){
            this->CameraConfigure.WhiteBalance = WhiteBalance;
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
        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_whitebalance)){
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

        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_exposure)){
            this->CameraConfigure.Exposure = Exposure;
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
        if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_exposure)){
            return true;
        }else{
            return false;
        }
    }
}

//设定拍照时的分辨率，返回值为bool
bool CameraControl::setHightAndWidth(cv::Size2i Size = cv::Size2i(720,1280)){
    if((cvSetCaptureProperty(CameraCapture,CV_CAP_PROP_FRAME_HEIGHT,Size.height) == 0) &&
            (cvSetCaptureProperty(CameraCapture,CV_CAP_PROP_FRAME_WIDTH,Size.width) == 0)){
        this->CameraConfigure.Size = Size;
        qDebug() << "Frame size set OK.";
        return true;
    }
    else{
        this->CameraConfigure.Size.height = 720;
        this->CameraConfigure.Size.width = 1280;
        qDebug() << "Frame size set Error.";
        return false;
    }
}


void CameraControl::run(){
    qDebug() << "camera :" << this->CameraNumber << " run() threadID :" << this->currentThreadId();

    //    this->update.stop();

    if(this->isUsed){
        QString fileName = "/dev/video";
        fileName = fileName + QString::number(this->CameraNumber);
        fd = open(fileName.toStdString().data(),O_RDWR);
        if(fd == -1){
            qDebug() << "Cannot Open Camera :" << this->CameraNumber;
        }else{
            setBrightness(this->CameraConfigure.Brightness);
            setContrast(this->CameraConfigure.Contrast);
            setExposure(this->CameraConfigure.Exposure);
            setGamma(this->CameraConfigure.Gamma);
            setHightAndWidth(this->CameraConfigure.Size);
            setWhiteBalance(this->CameraConfigure.WhiteBalance);
            setSaturation(this->CameraConfigure.Saturation);
            setHue(this->CameraConfigure.Hue);

            struct v4l2_control control_quality;
            control_quality.id = V4L2_CID_JPEG_COMPRESSION_QUALITY;
            control_quality.value = 100;
            if(fd && ioctl(fd,VIDIOC_S_CTRL,&control_quality)){
                qDebug() << "Set Camera to be best quality.";
            }

            if(myclose(fd)){
                qDebug() << "Camera :" << this->CameraNumber << " closed";
                fd = 0;
            }else{
                qDebug() << "Cannot close Camera :" << this->CameraNumber << " Please check.";
                myclose(fd);
                fd = 0;
            }

            this->CameraCapture = cvCaptureFromCAM(this->CameraNumber);
            IplImage *captureTemp = cvQueryFrame(CameraCapture);
            ((QList<IplImage *> *) &captured)->push_back(captureTemp);
            cvReleaseCapture(&CameraCapture);

            emit haveCaptured(this);
        }
    }else{
        ;
    }

    //    this->update.start(100);
}

//void CameraControl::updateUICamera(IplImage *img,int number){
//    number = this->CameraNumber;
//    emit this->updateUI(img,number);
//}

void CameraControl::onTimerUpdate(){
    //qDebug() << "camera view :" << this->CameraNumber << " timeout.";
    if(!this->isRunning()){
        //qDebug() << "camera view :" << this->CameraNumber << " release().";
        QsemTimeout->release(1);
    }else{
        QsemTimeout->acquire(QsemTimeout->available());
    }
    //    update.start(100);
}


getShortCut::getShortCut(int CameraNumber){
    this->CameraNUmber = CameraNumber;
}

getShortCut::~getShortCut(){
    ;
}

void getShortCut::run(){
    //qDebug() << "camera :" << this->CameraNUmber << " onTimerUpdate() threadID :" << this->currentThreadId();

    cv::VideoCapture CameraCapture;
    CameraCapture.open(this->CameraNUmber);
    if(CameraCapture.isOpened()){
        CameraCapture.set(CV_CAP_PROP_FRAME_HEIGHT,360);
        CameraCapture.set(CV_CAP_PROP_FRAME_WIDTH,480);
        cv::Mat captured;
        CameraCapture.grab();
        CameraCapture.read(captured);

        cv::cvtColor(captured,captured,CV_BGR2RGB);
        QImage get = QImage((uchar*)captured.data,captured.cols,captured.rows,captured.step,QImage::Format_RGB888);

        this->image = get.scaled(400,225);

        CameraCapture.release();

        //qDebug() << "resize width:" << (this->image).width() << " height:" << (this->image).height();

        emit updateUI((this->image),this->CameraNUmber);
    }else{
        qDebug() << "Cannot open Camera :" << this->CameraNUmber;
    }
}
