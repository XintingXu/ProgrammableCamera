#include <qimagewithiplimage.h>

QImage * IplImageToQImage(IplImage * cvimage){
    if (!cvimage){
        return NULL;
    }

    QImage* desImage = new QImage(cvimage->width,cvimage->height,QImage::Format_RGB32);

    for(int i = 0; i < cvimage->height; i++){
        for(int j = 0; j < cvimage->width; j++){
            int r=0,g=0,b=0;
            if(3 == cvimage->nChannels){
                b=(int)CV_IMAGE_ELEM(cvimage,uchar,i,j*3+0);
                g=(int)CV_IMAGE_ELEM(cvimage,uchar,i,j*3+1);
                r=(int)CV_IMAGE_ELEM(cvimage,uchar,i,j*3+2);
            }
            else if(1 == cvimage->nChannels){
                b=(int)CV_IMAGE_ELEM(cvimage,uchar,i,j);
                g=b;
                r=b;
            }

            desImage->setPixel(j,i,qRgb(r,g,b));
        }
    }
    return desImage;
}


IplImage * QImageToIplImage(QImage * qImage){
    int width = qImage->width();
    int height = qImage->height();
    CvSize Size;
    Size.height = height;
    Size.width = width;
    IplImage *IplImageBuffer = cvCreateImage(Size, IPL_DEPTH_8U, 3);
    for (int y = 0; y < height; ++y){
        for (int x = 0; x < width; ++x){
            QRgb rgb = qImage->pixel(x, y);
            cvSet2D(IplImageBuffer, y, x, CV_RGB(qRed(rgb), qGreen(rgb), qBlue(rgb)));
        }
    }
    return IplImageBuffer;
}
