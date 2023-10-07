#include <MyMindvisionCamera.h>

using namespace std;
int main() {
    MyMindvisionCamera Cam1(0);
    MyMindvisionCamera Cam2(1);
    bool run = true;
    int cnt = 0;
    int error_cnt = 0;
    while (run) {
        if(! (Cam1.grab() && Cam2.grab()))
        {
            cout << "GRAB ERROR\n";
            if(error_cnt++ > 10){
                run = false;
            }
            continue;
        }


        Mat img1, img2;
//        cout << "ok\n";
        Cam1.retrieve(img1);
        Cam2.retrieve(img2);
//        cout << "ok\n";

        if (img1.empty())
            continue;
        resize(img1, img1, Size(1280, 512));
        resize(img2, img2, Size(1280, 512));

        imshow("111", img1);
        imshow("222", img2);
        int k = waitKey(5);
        switch (k) {
            case 27:
                run = false;
                break;
            case 's':
                cout << "GRAB " << ++cnt << "PICS\n";
                imwrite("C:\\Projects\\MyMindvisionCamera\\pics\\CAM1\\pic_"+to_string(cnt)+".jpg", img1);
                imwrite("C:\\Projects\\MyMindvisionCamera\\pics\\CAM2\\pic_"+to_string(cnt)+".jpg", img2);
            default:
                break;
        }

    }

    return 0;
}
