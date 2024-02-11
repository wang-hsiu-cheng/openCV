#include <iostream>
#include <cstdlib>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main()
{
    Mat image = imread("smile.jpg"); // smile.jpg 為圖片檔名，名字自取。

    while (true)
    {
        imshow("live", image);
        if (waitKey(0) == 'N') // 執行程式後，點一下圖片，再輸入 N 就會結束迴圈。
            break;
    }
    return 0;
}
