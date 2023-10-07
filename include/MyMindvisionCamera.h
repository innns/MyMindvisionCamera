#ifndef MYMINDVISIONCAMERA_H
#define MYMINDVISIONCAMERA_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc/types_c.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <CameraApi.h>

using namespace std;
using namespace cv;


class MyMindvisionCamera
{
public:
    MyMindvisionCamera(int _id=-1);
    virtual ~MyMindvisionCamera();
    bool grab();
    bool retrieve(cv::Mat& _img);
    bool read(cv::Mat& _img);
    bool isOpened();
    bool release();

protected:
private:
    bool initCamera = false;
    // MindVision SDK parameter
    int iCameraCounts = 4; // zx edit: 初始改为4
    int iStatus = -1;
    tSdkCameraDevInfo tCameraEnumList[4]; // zx edit: 改为使用列表
    int iCameraId = 0;
    int hCamera; // 相机句柄
    int channel = 1;
    tSdkCameraCapbility tCapability; // 设备描述信息
    tSdkFrameHead sFrameInfo;
    BYTE *pbyBuffer;
    BYTE *g_pRgbBuffer; // 处理后数据缓存区
    char g_CameraName[64];
};

#endif // MYMINDVISIONCAMERA_H
