#include <opencv2/opencv.hpp>
// "C:\\Users\\kelly\\Downloads\\tunneltest2.jpg"
#define PATH "../pictureSource/warningSign.png"
#define PATH1 "testWarnSign1.png"

using namespace std;
using namespace cv;

int main()
{
    /// 需要調整的變數
    double epsilon = 13;        // DP Algorithm 的參數
    int minContour = 3;         // 邊數小於 minContour 會被遮罩
    int maxContour = 6;         // 邊數大於 maxContour
    double lowerBondArea = 500; // 面積低於 lowerBondArea 的輪廓會被遮罩

    Mat image = imread(PATH);

    if (image.empty())
    {
        std::cerr << "Could not open or find the image!" << std::endl;
        return -1;
    }

    Mat hsvImage;
    cvtColor(image, hsvImage, COLOR_BGR2HSV);

    // red range
    int hue_m = 0;
    int hue_M = 7;
    int sat_m = 129;
    int sat_M = 221;
    int val_m = 54;
    int val_M = 255;
    // PATH1
    // int hue_m = 49;
    // int hue_M = 179;
    // int sat_m = 94;
    // int sat_M = 255;
    // int val_m = 230;
    // int val_M = 255;
    Scalar lowerRed = Scalar(hue_m, sat_m, val_m);
    Scalar upperRed = Scalar(hue_M, sat_M, val_M);

    Mat redMask;
    inRange(hsvImage, lowerRed, upperRed, redMask);
    imshow("Red Mask", redMask);

    Mat result = Mat::zeros(image.size(), CV_8UC3);
    bitwise_and(image, image, result, redMask);

    cvtColor(result, result, COLOR_BGR2GRAY);
    threshold(result, result, 50, 255, THRESH_BINARY);

    Mat redEdges;
    // Canny(redMask, redEdges, 150, 200, 3);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(result, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    int triangleCount = 0;
    int detect = 0;

    // filter bad counters
    vector<vector<Point>> polyContours(contours.size()); // polyContours 用來存放折線點的集合

    // 2) 簡化邊緣： DP Algorithm
    for (size_t i = 0; i < contours.size(); i++)
    {
        approxPolyDP(Mat(contours[i]), polyContours[i], epsilon, true);
    }
    Mat dp_image = Mat::zeros(image.size(), CV_8UC3); // 初始化 Mat 後才能使用 drawContours
    drawContours(dp_image, polyContours, -2, Scalar(255, 0, 255), 1, 0);
    imshow("B", dp_image);
    // 3) 過濾不好的邊緣，用 badContour_mask 遮罩壞輪廓
    Mat badContour_mask = Mat::zeros(image.size(), CV_8UC3);
    for (size_t a = 0; a < polyContours.size(); a++)
    {
        // if 裡面如果是 true 代表該輪廓是不好的，會先被畫在 badContour_mask 上面
        if (polyContours[a].size() < minContour || polyContours[a].size() > maxContour ||
            contourArea(polyContours[a]) < lowerBondArea)
        {
            for (size_t b = 0; b < polyContours[a].size() - 1; b++)
            {
                line(badContour_mask, polyContours[a][b], polyContours[a][b + 1], Scalar(0, 255, 0), 3);
            }
            line(badContour_mask, polyContours[a][0], polyContours[a][polyContours[a].size() - 1], Scalar(0, 255, 0), 1, LINE_AA);
        }
    }
    // 4) 進行壞輪廓的遮罩
    Mat dp_optim_v1_image = Mat::zeros(image.size(), CV_8UC3);
    cvtColor(badContour_mask, badContour_mask, COLOR_BGR2GRAY);
    threshold(badContour_mask, badContour_mask, 0, 255, THRESH_BINARY_INV);
    bitwise_and(dp_image, dp_image, dp_optim_v1_image, badContour_mask);
    imshow("C", dp_optim_v1_image);
    // 5) 再從好的邊緣圖中找出邊緣
    cvtColor(dp_optim_v1_image, dp_optim_v1_image, COLOR_BGR2GRAY);
    threshold(dp_optim_v1_image, dp_optim_v1_image, 0, 255, THRESH_BINARY);
    vector<vector<Point>> contours2;
    vector<Vec4i> hierarchy2;
    findContours(dp_optim_v1_image, contours2, hierarchy2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    // 6) 簡化好輪廓 DP演算法
    vector<vector<Point>> polyContours2(contours2.size()); // 存放折線點的集合
    Mat dp_image_2 = Mat::zeros(dp_optim_v1_image.size(), CV_8UC3);
    for (size_t i = 0; i < contours2.size(); i++)
    {
        approxPolyDP(Mat(contours2[i]), polyContours2[i], epsilon, true);
        cout << polyContours2[i].size() << endl;
        if (polyContours2[i].size() == 3)
        {
            triangleCount++;
            detect = 1;
        }
    }
    drawContours(dp_image_2, polyContours2, -2, Scalar(255, 0, 255), 1, 0);
    imshow("D", dp_image_2);
    /**/
    // determine
    // for (const auto &contour : contours)
    // {
    //     vector<Point> approx;
    //     approxPolyDP(contour, approx, arcLength(contour, true) * 0.02, true);

    //     if (approx.size() == 3)
    //     {
    //         triangleCount++;
    //         detect = 1;
    //     }
    // }
    // output result
    cout << "Triangle Count: " << triangleCount << endl;

    // imshow("Red Mask", redMask);
    // imshow("Red Edges", redEdges);
    // imshow("B", dp_image);
    // imshow("C", dp_optim_v1_image);
    // imshow("D", dp_image_2);

    waitKey(0);
    destroyAllWindows();

    return 0;
}
