#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat contours_info(Mat image, vector<vector<Point>> contours)
{
    Mat info_image = Mat::zeros(image.size(), CV_8UC3);

    // 輪廓數量
    string name1 = "Number of Contours: " + to_string(contours.size());

    // 點數量
    int pt_count = 0;
    for (size_t a = 0; a < contours.size(); a++)
    {
        for (size_t b = 0; b < contours[a].size(); b++)
        {
            pt_count++;
        }
    }
    string name2 = "Number of Contours Points: " + to_string(pt_count);

    putText(info_image, name1, Point(10, 25), 0, 0.8, Scalar(0, 255, 0), 1, 1, false);
    putText(info_image, name2, Point(10, 60), 0, 0.8, Scalar(0, 255, 0), 1, 1, false);
    drawContours(info_image, contours, -2, Scalar(0, 0, 255), 1, 0);
    return info_image;
}

int main()
{
    string path = "..\\pictureSource\\left_sampl.jpg";
    Mat src = imread(path);
    Mat grayImg(src.rows, src.cols, CV_8UC1);
    Mat contoursImg(src.rows, src.cols, CV_8UC3);

    cvtColor(src, grayImg, COLOR_RGB2GRAY);
    threshold(grayImg, grayImg, 50, 255, THRESH_BINARY);

    // find contours
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(grayImg, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

    // draw contours
    Mat drawing = Mat::zeros(src.size(), CV_8UC3);
    for (size_t i = 0; i < contours.size(); i++)
    {
        Scalar color(0, 0, 255);
        drawContours(drawing, contours, -2, color, 1, 0, hierarchy);
    }

    while (waitKey(1) != 27)
    {
        imshow("source", src);
        imshow("gray", grayImg);
        imshow("Contours", contours_info(contoursImg, contours));
    }

    return 0;
}