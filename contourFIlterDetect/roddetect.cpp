#include <opencv2/opencv.hpp>

#define PATH "../pictureSource/rodSign.png"

using namespace std;
using namespace cv;

int main()
{
    double epsilon = 13;        // DP Algorithm 的參數
    int minContour = 3;         // 邊數小於 minContour 會被遮罩
    int maxContour = 5;         // 邊數大於 maxContour
    double lowerBondArea = 200; // 面積低於 lowerBondArea 的輪廓會被遮罩

    Mat image = imread(PATH);

    if (image.empty())
    {
        cerr << "Could not open or find the image!" << endl;
        return -1;
    }

    Mat hsvImage;
    cvtColor(image, hsvImage, COLOR_BGR2HSV);

    // int hue_m = 49;
    // int hue_M = 179;
    // int sat_m = 94;
    // int sat_M = 255;
    // int val_m = 230;
    // int val_M = 255;
    int hue_m = 155;
    int hue_M = 179;
    int sat_m = 158;
    int sat_M = 255;
    int val_m = 0;
    int val_M = 255;
    Scalar lowerRed = Scalar(hue_m, sat_m, val_m);
    Scalar upperRed = Scalar(hue_M, sat_M, val_M);

    Mat redMask;
    inRange(hsvImage, lowerRed, upperRed, redMask);
    imshow("Red Mask", redMask);

    Mat result = Mat::zeros(image.size(), CV_8UC3);
    bitwise_and(image, image, result, redMask);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(redMask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    int rodCount = 0;

    // 過濾不好的輪廓
    vector<vector<Point>> polyContours(contours.size());

    for (size_t i = 0; i < contours.size(); i++)
    {
        approxPolyDP(Mat(contours[i]), polyContours[i], epsilon, true);
    }

    // 進行輪廓過濾
    Mat badContourMask = Mat::zeros(image.size(), CV_8UC3);

    for (size_t a = 0; a < polyContours.size(); a++)
    {
        if (polyContours[a].size() < minContour || polyContours[a].size() > maxContour ||
            contourArea(polyContours[a]) < lowerBondArea)
        {
            for (size_t b = 0; b < polyContours[a].size() - 1; b++)
            {
                line(badContourMask, polyContours[a][b], polyContours[a][b + 1], Scalar(0, 255, 0), 3);
            }
            line(badContourMask, polyContours[a][0], polyContours[a][polyContours[a].size() - 1], Scalar(0, 255, 0), 1, LINE_AA);
        }
    }

    Mat optimizedImage = Mat::zeros(image.size(), CV_8UC3);
    cvtColor(badContourMask, badContourMask, COLOR_BGR2GRAY);
    threshold(badContourMask, badContourMask, 0, 255, THRESH_BINARY_INV);
    bitwise_and(result, result, optimizedImage, badContourMask);

    cvtColor(optimizedImage, optimizedImage, COLOR_BGR2GRAY);
    threshold(optimizedImage, optimizedImage, 0, 255, THRESH_BINARY);

    vector<vector<Point>> filteredContours;
    vector<Vec4i> hierarchy2;
    findContours(optimizedImage, filteredContours, hierarchy2, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 計算桿子數量
    for (size_t i = 0; i < filteredContours.size(); i++)
    {
        if (filteredContours[i].size() >= minContour && filteredContours[i].size() <= maxContour &&
            contourArea(filteredContours[i]) >= lowerBondArea)
        {
            rodCount++;
        }
    }

    cout << "Total rods detected: " << rodCount << endl;

    // 使用紅色遮罩直接找到紅色部分的輪廓
    Mat redContoursImage = Mat::zeros(image.size(), CV_8UC3);
    findContours(redMask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    drawContours(redContoursImage, contours, -1, Scalar(0, 255, 0), 2);

    imshow("Red Contours", redContoursImage);

    // 查找紅色遮罩中的輪廓
    findContours(redMask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

    // 計算輪廓點數和印出每個輪廓的點座標
    size_t totalPoints = 0; // 總點數
    for (size_t a = 0; a < contours.size(); a++)
    {
        // 計算輪廓點個數
        size_t contourPoints = contours[a].size();
        totalPoints += contourPoints;

        printf("Contour %zu points: %zu\n", a, contourPoints);

        // 印出每個輪廓的點座標
        for (size_t b = 0; b < contourPoints; b++)
        {
            Point point = contours[a][b];
            printf("Point %zu: (%d, %d)\n", b, point.x, point.y);
        }
    }

    printf("Total points: %zu\n", totalPoints);

    waitKey(0);
    destroyAllWindows();

    return 0;
}
