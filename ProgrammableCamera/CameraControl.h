#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QThread>
#include <opencv2/opencv.hpp>
#include <opencv2/ocl/ocl.hpp>
#include <QDebug>
#include <iostream>

/*
摄像头控制类
设定摄像头的编号、分辨率、曝光度等参数，达到拍照的设定效果
设计定于单独的线程运行，且设定运行在一个CPU核中。

作者：徐欣廷
创建时间：2017-02-22 09:54:17
上次修改时间：2017-02-22 09:54:30
*/
class CameraControl:public QThread{		//多线程运行
private:
    int CameraNumber;		//摄像头的编号
    cv::VideoCapture *CameraCapture;	//摄像头捕获类
    struct CameraConfigure{		//摄像头参数结构体
        cv::Size2i Size;	//照片的分辨率
        int Brightness;		//拍照时的白平衡
        int Contrast;		//拍照时的对比度
        int Saturation;		//拍照时的色饱和度
        int Tone;			//拍照时的色调
        int Resolution;		//拍照时的清晰度
        double Gamma;		//拍照时的Gamma值
        int WhiteBalance;	//拍照时的白平衡
        int Exposure;		//拍照时的曝光度
    };
public:
    CameraControl(int CameraNumber);			//构造函数
    ~CameraControl();						//析构函数
    bool setCamera(int CameraNumber);		//设定摄像头的编号，返回值为bool
    bool setBrightness(int Brightness);		//设定拍照时的白平衡，返回值为bool
    bool setContrast(int Contrast);			//设定拍照时的对比度，返回值为bool
    bool setSaturation(int Saturation);		//设定拍照时的色饱和度，返回值为bool
    bool setTone(int Tone);					//设定拍照时的色调，返回值为bool
    bool setResolution(int Resolution);		//设定拍照时的清晰度，返回值为bool
    bool setGamma(double Gamma);			//设定拍照时的Gamma值，返回值为bool
    bool setWhiteBalance(int WhiteBalance);	//设定拍照时的白平衡，返回值为bool
    bool setExposure(int Exposure);			//设定拍照时的曝光度，返回值为bool
    bool setHightAndWidth(cv::Size2i Size);	//设定拍照时的分辨率，返回值为bool
};


#endif // CAMERACONTROL_H
