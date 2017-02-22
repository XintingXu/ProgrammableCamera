#ifndef CAMERACONTROL_H
#define CAMERACONTROL_H

#include <QThread>
#include <opencv2/opencv.hpp>
#include <opencv2/ocl/ocl.hpp>
#include <QDebug>
#include <iostream>

class CameraControl:public QThread{
private:
    int CameraNumber;
    cv::VideoCapture *CameraCapture;
    struct CameraConfigure{
        cv::Size2i Size;
        int Brightness;
        int Contrast;
        int Saturation;
        int Tone;
        int Resolution;
        double Gamma;
        int WhiteBalance;
        int Exposure;
    };
public:
    CameraNumber(int CameraNumber);
    ~CameraNumber();
    bool setCamera(int CameraNumber);
    bool setBrightness(int Brightness);
    bool setContrast(int Contrast);
    bool setSaturation(int Saturation);
    bool setTone(int Tone);
    bool setResolution(int Resolution);
    bool setGamma(double Gamma);
    bool setWhiteBalance(int WhiteBalance);
    bool setExposure(int Exposure);
    bool setHightAndWidth(cv::Size2i Size);
};


#endif // CAMERACONTROL_H
