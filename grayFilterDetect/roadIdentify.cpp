#include <opencv2/opencv.hpp>
#include <iostream>
#define PATH "C:/Users/TWang/Desktop/coding/openCV/pictureSource/roadline_test3.jpg"
using namespace std;
using namespace cv;

bool isPrinted = false;

void GetROI(Mat src, Mat &ROI)
{
    int width = src.cols;
    int height = src.rows;

    // get ROI area, focus on specific part to identify
    vector<Point> pts;
    Point ptA((width / 8) * 2, (height / 20) * 10);
    Point ptB((width / 8) * 2, (height / 20) * 15);
    Point ptC((width / 10) * 4, (height / 20) * 19);
    Point ptD((width / 10) * 5, (height / 20) * 19);
    Point ptE((width / 8) * 5, (height / 20) * 15);
    Point ptF((width / 8) * 5, (height / 20) * 10);
    pts = {ptA, ptB, ptC, ptD, ptE, ptF};

    // copy the part of photo we want
    vector<vector<Point>> ppts;
    ppts.push_back(pts);

    Mat mask = Mat::zeros(src.size(), src.type());
    fillPoly(mask, ppts, Scalar::all(255));

    src.copyTo(ROI, mask);
}

Mat FiltGraph(Mat src, Mat &ROI)
{
    Mat img_hsv, mask, result;
    cvtColor(src, img_hsv, COLOR_BGR2HSV);

    // white range
    int hue_m = 0;
    int hue_M = 173;
    int sat_m = 0;
    int sat_M = 190;
    int val_m = 191;
    int val_M = 226;

    Scalar lower(hue_m, sat_m, val_m);
    Scalar upper(hue_M, sat_M, val_M);
    inRange(img_hsv, lower, upper, mask);

    ROI = Mat::zeros(src.size(), CV_8UC3);
    bitwise_and(src, src, ROI, mask);
    // imshow("Letter Filted", ROI);
    return result;
}

void DetectRoadLine(Mat src, Mat &ROI)
{
    Mat gray;
    cvtColor(ROI, ROI, COLOR_BGR2GRAY);

    Mat thresh;
    threshold(ROI, thresh, 50, 255, THRESH_BINARY);
    // imshow("gray", thresh);

    vector<Point> left_line;
    vector<Point> right_line;

    // left road line
    for (int i = thresh.rows / 2; i < thresh.rows; i++)
    {
        for (int j = 0; j < thresh.cols / 2; j++)
        {
            if (thresh.at<uchar>(i, j) == 255)
            {
                left_line.push_back(Point(j, i));
                break;
            }
        }
    }
    // right road line
    for (int i = thresh.rows / 2; i < thresh.rows; i++)
    {
        for (int j = thresh.cols - 1; j > thresh.cols / 2; j--)
        {
            if (thresh.at<uchar>(i, j) == 255)
            {
                right_line.push_back(Point(j, i));
                break;
            }
        }
    }

    // draw the road line on the photo
    if (left_line.size() > 0 && right_line.size() > 0)
    {
        double times = pow(10, 2);
        Point T_L = (left_line[0]);
        Point B_L = (left_line[left_line.size() - 1]);
        Point T_R = (right_line[0]);
        Point B_R = (right_line[right_line.size() - 1]);
        Point center_begin_point = Point((B_L.x + B_R.x) / 2, B_L.y);
        Point center_end_point = Point((T_L.x + T_R.x) / 2, T_L.y);
        double slop = (center_begin_point.y - center_end_point.y) / (center_end_point.x - center_begin_point.x);
        if (!isPrinted)
        {
            isPrinted = true;
            printf("b_l:%.2f,%.2f t_l:%.2f,%.2f\n", B_L.x * times, B_L.y * times, T_L.x * times, T_L.y * times);
            printf("b_c : %.2f, %.2f t_c : %.2f, %.2f\n", center_begin_point.x * times, center_begin_point.y * times, center_end_point.x * times, center_end_point.y * times);
            printf("b_r : %.2f, %.2f t_r : %.2f, %.2f\n", B_R.x * times, B_R.y * times, T_R.x * times, T_R.y * times);
        }

        circle(src, B_L, 10, Scalar(0, 0, 255), -1);
        circle(src, T_L, 10, Scalar(0, 255, 0), -1);
        circle(src, T_R, 10, Scalar(255, 0, 0), -1);
        circle(src, B_R, 10, Scalar(0, 255, 255), -1);
        circle(src, center_begin_point, 10, Scalar(255, 0, 0), -1);
        circle(src, center_end_point, 10, Scalar(0, 255, 255), -1);

        vector<Point> pts;
        pts = {B_L, T_L, T_R, B_R};
        vector<vector<Point>> ppts;
        ppts.push_back(pts);
        fillPoly(src, ppts, Scalar(100, 201, 201));

        line(src, Point(B_L), Point(T_L), Scalar(0, 255, 0), 10);
        line(src, Point(B_R), Point(T_R), Scalar(0, 255, 0), 10);
        line(src, Point(center_begin_point), Point(center_end_point), Scalar(0, 255, 255), 10);
    }
}

int main()
{
    VideoCapture cap(0); // 鏡頭編號依序從 012...
    Mat img;

    if (!cap.isOpened())
    { // 確認有連線到該編號的鏡頭
        cout << "Cannot open capture\n";
    }

    while (true)
    {
        // resize(src, src, Size(src.cols / 2, src.rows / 2));
        bool ret = cap.read(img);
        if (!ret)
        {
            cout << "Cant receive frame\n";
            break;
        }

        img = imread(PATH);

        Mat original_image = img.clone();
        // GetROI(original_image, img);
        FiltGraph(original_image, img);
        DetectRoadLine(original_image, img);
        imshow("frame", original_image);
        if (waitKey(1) == 'q')
            break;
    }
    return 0;
}