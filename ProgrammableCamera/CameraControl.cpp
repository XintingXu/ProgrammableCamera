#include "CameraControl.h"

/*
CameraControl.h的函数实现

作者：徐欣廷
创建时间：2017-02-22
修改时间：2017-02-22
*/


//CameraControl的构造函数，需要传入摄像头的编号作为参数，默认设为0
CameraControl::CameraControl(int CameraNumber = 0){
    ;
}


//CameraControl的析构函数，释放内存
CameraControl::~CameraControl(){
    ;
}

//设定摄像头的编号，返回值为bool
bool CameraControl::setCamera(int CameraNumber){
    return true;
}

//设定拍照时的亮度，返回值为bool
bool CameraControl::setBrightness(int Brightness){
    return true;
}

//设定拍照时的对比度，返回值为bool
bool CameraControl::setContrast(int Contrast){
    return true;
}

//设定拍照时的色饱和度，返回值为bool
bool CameraControl::setSaturation(int Saturation){
    return true;
}

//设定拍照时的色调，返回值为bool
bool CameraControl::setTone(int Tone){
    return true;
}

//设定拍照时的清晰度，返回值为bool
bool CameraControl::setResolution(int Resolution){
    return true;
}

//设定拍照时的Gamma值，返回值为bool
bool CameraControl::setGamma(double Gamma){
    return true;
}

//设定拍照时的白平衡，返回值为bool
bool CameraControl::setWhiteBalance(int WhiteBalance){
    return true;
}

//设定拍照时的曝光度，返回值为bool
bool CameraControl::setExposure(int Exposure){
    return true;
}

//设定拍照时的分辨率，返回值为bool
bool CameraControl::setHightAndWidth(cv::Size2i Size){
    return true;
}
