#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QThread>
#include <opencv2/opencv.hpp>
#include <QDebug>
#include <iostream>
#include <pthread.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <QTimerEvent>
#include <QTimer>
#include <QList>
#include <QSemaphore>
#include <QImage>

struct CameraControlValue{		//摄像头参数结构体
    cv::Size2i Size=cv::Size2i(720,1280);	//照片的分辨率
    int Brightness = -8193;		//拍照时的亮度
    int Contrast = 57343;		//拍照时的对比度
    int Saturation = 57343;		//拍照时的色饱和度
    int Hue = -8193;			//拍照时的色调
    int Resolution = 57343;		//拍照时的清晰度
    int Gamma = 57343;		//拍照时的Gamma值
    int WhiteBalance = 0;	//拍照时的白平衡
    int Exposure = 0;		//拍照时的曝光度
};

class getShortCut;

/*
 *
 * platform-3f980000.usb-usb-0:1.4.2:1.0-video-index0   //normal camera using MJPEG encode
 * platform-3f980000.usb-usb-0:1.4.2:1.0-video-index1   //normal camera using YUYV encode
 * platform-3f980000.usb-usb-0:1.4.3:1.0-video-index0   //IR camera using MJPEG encode
 * platform-3f980000.usb-usb-0:1.4.3:1.0-video-index1   //IR camera using YUYV encode
 *
 *
 */

/*
摄像头控制类
设定摄像头的编号、分辨率、曝光度等参数，达到拍照的设定效果
设计定于单独的线程运行，且设定运行在一个CPU核中。

作者：徐欣廷
创建时间：2017-02-22
上次修改时间：2017-02-22
*/
class CameraControl:public QThread{		//多线程运行
    Q_OBJECT

private:
    int CameraNumber;		//摄像头的编号
    cpu_set_t setMask;      //mask used to configure which cpu to run
    int fd;                 //file handle
    cv::VideoCapture CameraCapture;

public:
    QList<CameraControlValue> CameraConfigure;
    QList<cv::Mat> captured;

    CameraControl(int CameraNumber);		//构造函数
    ~CameraControl();						//析构函数
    bool setCamera(int CameraNumber);		//设定摄像头的编号，返回值为bool
    bool setBrightness(int Brightness);		//设定拍照时的亮度，返回值为bool
    bool setContrast(int Contrast);			//设定拍照时的对比度，返回值为bool
    bool setSaturation(int Saturation);		//设定拍照时的色饱和度，返回值为bool
    bool setHue(int Hue);					//设定拍照时的色调，返回值为bool
    bool setResolution(int Resolution);		//设定拍照时的清晰度，返回值为bool
    bool setGamma(int Gamma);			//设定拍照时的Gamma值，返回值为bool
    bool setWhiteBalance(int WhiteBalance);	//设定拍照时的白平衡，返回值为bool
    bool setExposure(int Exposure);			//设定拍照时的曝光度，返回值为bool
    bool setHightAndWidth(cv::Size2i Size);	//设定拍照时的分辨率，返回值为bool
    bool setConfighration();

    int getCameraNumber();
    volatile bool isUsed = false;

    void run();
};

class getShortCut:public QThread{
    Q_OBJECT
public:
    void run();
    getShortCut(int CameraNumber);
    ~getShortCut();
private:
    int CameraNUmber;
    QImage image;
signals:
    void updateUI(QImage,int);
};

#endif // CAMERACONTROL_H
