#ifndef QIMAGEWITHIPLIMAGE
#define QIMAGEWITHIPLIMAGE

#include <QImage>
#include <opencv2/opencv.hpp>

QImage * IplImageToQImage(IplImage * cvImage);
IplImage * QImageToIplImage(QImage * qImage);

#endif // QIMAGEWITHIPLIMAGE
