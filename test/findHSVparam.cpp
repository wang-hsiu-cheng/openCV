/** 尋找 inrange 的 hsv 參數 **/
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// 字母過濾
void filt_letter(Mat img);

//// 對單一圖片過濾 ////
int main()
{
    // 圖片路徑
    string path = "../pictureSource/blueHSVtest.png";
    Mat src = imread(path);

    filt_letter(src);
    return 0;
}

void filt_letter(Mat img)
{
    Mat img_hsv, mask, result;

    cvtColor(img, img_hsv, COLOR_BGR2HSV);

    namedWindow("track_bar");
    resizeWindow("track_bar", 500, 500);

    createTrackbar("Hue Min", "track_bar", 0, 179);
    createTrackbar("Hue Max", "track_bar", 0, 179);
    createTrackbar("Sat Min", "track_bar", 0, 255);
    createTrackbar("Sat Max", "track_bar", 0, 255);
    createTrackbar("Val Min", "track_bar", 0, 255);
    createTrackbar("Val Max", "track_bar", 0, 255);

    setTrackbarPos("Hue Min", "track_bar", 0);
    setTrackbarPos("Hue Max", "track_bar", 179);
    setTrackbarPos("Sat Min", "track_bar", 0);
    setTrackbarPos("Sat Max", "track_bar", 255);
    setTrackbarPos("Val Min", "track_bar", 0);
    setTrackbarPos("Val Max", "track_bar", 255);

    // 按 q 退出視窗
    while (waitKey(1) != 'q')
    {
        int hue_m = getTrackbarPos("Hue Min", "track_bar");
        int hue_M = getTrackbarPos("Hue Max", "track_bar");
        int sat_m = getTrackbarPos("Sat Min", "track_bar");
        int sat_M = getTrackbarPos("Sat Max", "track_bar");
        int val_m = getTrackbarPos("Val Min", "track_bar");
        int val_M = getTrackbarPos("Val Max", "track_bar");

        Scalar lower(hue_m, sat_m, val_m);
        Scalar upper(hue_M, sat_M, val_M);
        inRange(img_hsv, lower, upper, mask);

        imshow("Img", img);
        result = Mat::zeros(img.size(), CV_8UC3);

        // 遮罩原圖
        bitwise_and(img, img, result, mask);
        imshow("Result", result);
    }
}