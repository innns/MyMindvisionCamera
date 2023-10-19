#include "MyMindvisionCamera.h"

MyMindvisionCamera::MyMindvisionCamera(int _id) {
    cout << "try to open camera......" << endl;
    // 初始化SDK 1表示使用中文
    CameraSdkInit(1);
    // 枚举设备，并建立设备列表
    // zx edit: 改为列表
    CameraEnumerateDevice(tCameraEnumList, &iCameraCounts);
    printf("state = %d\n", iStatus);
    printf("count = %d\n", iCameraCounts);
    // 没有连接设备
    if (iCameraCounts == 0) {
        std::cout << "No camera\n";
        return;
    }
    // 枚举设备
    for (int cnt = 0; cnt < iCameraCounts; ++cnt) {
        printf("Cam[%d]: ", cnt);
        std::cout << tCameraEnumList[cnt].acProductName << "\n";
    }
    iCameraId = _id;
    // 如果大于一个相机且没有指定_id
    if (iCameraCounts > 1 && iCameraId < 0) {
        std::cout << "Choose Cam id: ";
        std::cin >> iCameraId;
    }
    // 指定了不合法的相机
    if (iCameraId > iCameraCounts - 1 || iCameraId < 0) {
        std::cout << "Use Cam[0]";
        iCameraId = 0;
    }
    // 相机初始化。初始化成功后，才能调用任何其他相机相关的操作接口
    iStatus = CameraInit(&tCameraEnumList[iCameraId], -1, -1, &hCamera);
    // 初始化失败
    if (iStatus != CAMERA_STATUS_SUCCESS) {
        printf("Camera init failed, state = %d\n", iStatus);
        return;
    }

    // 获得相机的特性描述结构体。该结构体中包含了相机可设置的各种参数的范围信息。决定了相关函数的参数
    CameraGetCapability(hCamera, &tCapability);

    /*
     * 记录报错问题：g_pRgbBuffer = (BYTE *)CameraAlignMalloc(tCapability.sResolutionRange.iHeightMax * tCapability.sResolutionRange.iWidthMax * 3, 16)
     *
     * CameraAlignMalloc函数为申请内存大小，但是官方解析出来的是单目相机的分辨率（1280*1024），如果组成双目相机，分辨率应该为：2560*1024.开辟的内存应该需要再乘于2.
     *
     * */
    g_pRgbBuffer = (BYTE *) CameraAlignMalloc(
            tCapability.sResolutionRange.iHeightMax * tCapability.sResolutionRange.iWidthMax * 3 * 2, 16);
    cout << tCapability.sResolutionRange.iHeightMax << " " << tCapability.sResolutionRange.iWidthMax << endl;
    /*让SDK进入工作模式，开始接收来自相机发送的图像
    数据。如果当前相机是触发模式，则需要接收到
    触发帧以后才会更新图像。    */
    strcpy_s(g_CameraName, tCameraEnumList[iCameraId].acFriendlyName);
    CameraCreateSettingPage(hCamera, NULL,
                            g_CameraName, NULL, NULL, 0); //"通知SDK内部建该相机的属性页面";
    CameraPlay(hCamera);
    CameraShowSettingPage(hCamera, FALSE); // TRUE显示相机配置界面。FALSE则隐藏。

    /*其他的相机参数设置
    例如 CameraSetExposureTime   CameraGetExposureTime  设置/读取曝光时间
            CameraSetImageResolution  CameraGetImageResolution 设置/读取分辨率
            CameraSetGamma、CameraSetConrast、CameraSetGain等设置图像伽马、对比度、RGB数字增益等等。
            更多的参数的设置方法，，参考MindVision_Demo。本例程只是为了演示如何将SDK中获取的图像，转成OpenCV的图像格式,以便调用OpenCV的图像处理函数进行后续开发
    */

    if (tCapability.sIspCapacity.bMonoSensor) {
        channel = 1;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_MONO8);
    } else {
        channel = 3;
        CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_BGR8);
    }
    // 设置为手动曝光,设置曝光时间、模拟增益、伽马值和对比度
    // TODO:设置相机参数
    //  zx edit: 使用 CameraInit(&tCameraEnumList[iCameraId], -1, -1, &hCamera); 会保存上一次的参数
    CameraSetFrameSpeed(hCamera, 2);//设置为高速模式。
    CameraSetAeState(hCamera, false);
    if (channel == 1) {
        CameraSetExposureTime(hCamera, 15000); //单位us
        CameraSetAnalogGain(hCamera, 3); //模拟增益倍数
        // CameraSetContrast(hCamera, 150); //对比度越大，会使图像黑的区域越黑，白的区域越白
    } else {
        CameraSetExposureTime(hCamera, 10000); //单位us
        CameraSetAnalogGain(hCamera, 2); //模拟增益倍数
        // CameraSetContrast(hCamera, 150); //对比度越大，会使图像黑的区域越黑，白的区域越白
    }
    initCamera = true;
}

MyMindvisionCamera::~MyMindvisionCamera() {
    release();
}

bool MyMindvisionCamera::grab() {
    return (initCamera) && (CameraGetImageBuffer(hCamera, &sFrameInfo, &pbyBuffer, 1000) == CAMERA_STATUS_SUCCESS);
}

bool MyMindvisionCamera::retrieve(cv::Mat &_img) {
    try {
        CameraImageProcess(hCamera, pbyBuffer, g_pRgbBuffer, &sFrameInfo);
        CameraFlipFrameBuffer(g_pRgbBuffer, &sFrameInfo, 1); // 图像翻转
        CameraSetMirror(hCamera, 0, 0);                      // 图像镜像
        cv::Mat matImage(
                Size(sFrameInfo.iWidth, sFrameInfo.iHeight),
                channel == 1 ? CV_8UC1 : CV_8UC3,
                g_pRgbBuffer);
        _img = matImage;
        // NOTE: 现在不用换BGR了 原本就是正常的
//        cvtColor(matImage, _img, cv::COLOR_RGB2BGR); // 缓冲区图像格式为RGB, 需改为opencv常用的BGR
        // 在成功调用CameraGetImageBuffer后，必须调用CameraReleaseImageBuffer来释放获得的buffer。
        // 否则再次调用CameraGetImageBuffer时，程序将被挂起一直阻塞，直到其他线程中调用CameraReleaseImageBuffer来释放了buffer
        CameraReleaseImageBuffer(hCamera, pbyBuffer);
        return true;
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return false;
    }
}

bool MyMindvisionCamera::read(cv::Mat &_img) {
    return (grab()) ? (retrieve(_img)) : false;
}

bool MyMindvisionCamera::isOpened() {
    return initCamera;
}

bool MyMindvisionCamera::release() {
    if (initCamera) {
        CameraUnInit(hCamera);
        initCamera = false;
    }

    if (g_pRgbBuffer) {
        CameraAlignFree(g_pRgbBuffer);
        g_pRgbBuffer = NULL;
    }
}