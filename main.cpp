#include <MyMindvisionCamera.h>
#include <direct.h>
#include <io.h>

using namespace std;

int main() {

    String dir1 = "C:\\Projects\\MyMindvisionCamera\\pics\\CAM1";
    String dir2 = "C:\\Projects\\MyMindvisionCamera\\pics\\CAM2";

    if (access(dir1.c_str(), 0) == -1) { //判断该文件夹是否存在
#ifdef WIN32:
        int flag1 = mkdir(dir1.c_str());
#endif
        if (flag1 == 0) {  //创建成功
            std::cout << "Create CAM1 dir successfully." << std::endl;
        } else { //创建失败
            std::cout << "Fail to create directory." << std::endl;
            throw std::exception();
        }
    } else {
        std::cout << "CAM1 already exists." << std::endl;
    }

    if (access(dir2.c_str(), 0) == -1) { //判断该文件夹是否存在
#ifdef WIN32:
        int flag2 = mkdir(dir2.c_str());
#endif
        if (flag2 == 0) {  //创建成功
            std::cout << "Create CAM2 dir successfully." << std::endl;
        } else { //创建失败
            std::cout << "Fail to create directory." << std::endl;
            throw std::exception();
        }
    } else {
        std::cout << "CAM2 already exists." << std::endl;
    }


    MyMindvisionCamera Cam1(0);
    MyMindvisionCamera Cam2(1);
    bool run = true;
    int cnt = 0;
    int error_cnt = 0;
    while (run) {
        if (!(Cam1.grab() && Cam2.grab())) {
            cout << "GRAB ERROR\n";
            if (error_cnt++ > 10) {
                run = false;
            }
            continue;
        }


        Mat img1, img2, img_1, img_2;
//        cout << "ok\n";
        Cam1.retrieve(img1);
        Cam2.retrieve(img2);
//        cout << "ok\n";

        if (img1.empty())
            continue;
        resize(img1, img_1, Size(1280, 512));
        resize(img2, img_2, Size(1280, 512));

        imshow("111", img_1);
        imshow("222", img_2);

        int k = waitKey(5);
        switch (k) {
            case 27:
                run = false;
                break;
            case 's':
                cout << "GRAB " << ++cnt << "PICS\n";
                imwrite("C:\\Projects\\MyMindvisionCamera\\pics\\CAM1\\pic_" + to_string(cnt) + ".jpg", img1);
                imwrite("C:\\Projects\\MyMindvisionCamera\\pics\\CAM2\\pic_" + to_string(cnt) + ".jpg", img2);
            default:
                break;
        }


    }

    return 0;
}
