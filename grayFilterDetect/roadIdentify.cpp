#include <opencv2/opencv.hpp>
#include <iostream>
#define PATH "../pictureSource/roadline_test3.jpg"
#define PI 3.141592653589793238462643383279502884
using namespace std;
using namespace cv;

bool isPrinted = false;
double slop = 0;

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

void FiltGraph(Mat src, Mat &filteredImg, char color)
{
    Mat img_hsv, mask;
    cvtColor(src, img_hsv, COLOR_BGR2HSV);
    int hue_m, hue_M, sat_m, sat_M, val_m, val_M;

    if (color == 'y')
    {
        // yellow range
        hue_m = 14;
        hue_M = 78;
        sat_m = 48;
        sat_M = 255;
        val_m = 108;
        val_M = 255;
    }
    else if (color == 'w')
    {
        // white range
        hue_m = 0;
        hue_M = 179;
        sat_m = 0;
        sat_M = 40;
        val_m = 175;
        val_M = 255;
    }

    Scalar lower(hue_m, sat_m, val_m);
    Scalar upper(hue_M, sat_M, val_M);
    inRange(img_hsv, lower, upper, mask);
    filteredImg = Mat::zeros(src.size(), CV_8UC3);
    bitwise_and(src, src, filteredImg, mask);
    string pictureName("Filted");
    pictureName.push_back(color);
    // imshow(pictureName, filteredImg);
}

void DetectRoadLine(Mat src, Mat &yellowImg, Mat &whiteImg)
{
    Mat thresh_yellow, thresh_white;

    cvtColor(yellowImg, yellowImg, COLOR_BGR2GRAY);
    cvtColor(whiteImg, whiteImg, COLOR_BGR2GRAY);
    threshold(yellowImg, thresh_yellow, 50, 255, THRESH_BINARY);
    threshold(whiteImg, thresh_white, 50, 255, THRESH_BINARY);
    // imshow("yellow_to_gray", thresh_yellow);
    // imshow("white_to_gray", thresh_white);

    vector<Point> left_line;
    vector<Point> right_line;

    // left road line
    for (int i = thresh_yellow.rows / 2; i < thresh_yellow.rows; i++)
    {
        for (int j = 0; j < thresh_yellow.cols / 2; j++)
        {
            if (thresh_yellow.at<uchar>(i, j) == 255)
            {
                left_line.push_back(Point(j, i));
                break;
            }
        }
    }
    // right road line
    for (int i = thresh_white.rows / 2; i < thresh_white.rows; i++)
    {
        for (int j = thresh_white.cols - 1; j > thresh_white.cols / 2; j--)
        {
            if (thresh_white.at<uchar>(i, j) == 255)
            {
                right_line.push_back(Point(j, i));
                break;
            }
        }
    }

    // draw the road line on the photo
    if (left_line.size() > 0 && right_line.size() > 0)
    {
        double times = pow(10, 0);
        Point T_L = (left_line[0]);
        Point B_L = (left_line[left_line.size() - 1]);
        Point T_R = (right_line[0]);
        Point B_R = (right_line[right_line.size() - 1]);
        Point center_begin_point = Point((B_L.x + B_R.x) / 2, B_L.y);
        Point center_end_point = Point((T_L.x + T_R.x) / 2, T_L.y);
        if (center_end_point.x - center_begin_point.x == 0)
            slop = 0;
        else
            slop = atan((center_begin_point.y - center_end_point.y) / (center_end_point.x - center_begin_point.x)) - PI / 2;
        if (!isPrinted)
        {
            isPrinted = true;
            // printf("b_l:%.2f,%.2f t_l:%.2f,%.2f\n", B_L.x * times, B_L.y * times, T_L.x * times, T_L.y * times);
            // printf("b_c : %.2f, %.2f t_c : %.2f, %.2f\n", center_begin_point.x * times, center_begin_point.y * times, center_end_point.x * times, center_end_point.y * times);
            // printf("b_r : %.2f, %.2f t_r : %.2f, %.2f\n", B_R.x * times, B_R.y * times, T_R.x * times, T_R.y * times);
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
    else if (left_line.size() > 0 && right_line.size() == 0)
    {
        Point T_L = (left_line[0]);
        Point B_L = (left_line[left_line.size() - 1]);
        Point center_begin_point = Point(B_L.x, B_L.y);
        Point center_end_point = Point(T_L.x, T_L.y);
        if (center_end_point.x - center_begin_point.x == 0)
            slop = 0;
        else
            slop = atan((center_begin_point.y - center_end_point.y) / (center_end_point.x - center_begin_point.x)) - PI / 2;
        if (!isPrinted)
        {
            isPrinted = true;
            // printf("b_c : %.2f, %.2f t_c : %.2f, %.2f\n", center_begin_point.x, center_begin_point.y, center_end_point.x, center_end_point.y);
            // printf("b_l : %.2f, %.2f t_l : %.2f, %.2f\n", B_L.x, B_L.y, T_L.x, T_L.y);
        }
        circle(src, center_begin_point, 10, Scalar(255, 0, 0), -1);
        circle(src, center_end_point, 10, Scalar(0, 255, 255), -1);

        vector<Point> pts;
        pts = {T_L, B_L};
        vector<vector<Point>> ppts;
        ppts.push_back(pts);

        line(src, Point(B_L), Point(T_L), Scalar(0, 255, 0), 10);
        line(src, Point(center_begin_point), Point(center_end_point), Scalar(0, 255, 255), 10);
    }
    else if (left_line.size() == 0 && right_line.size() > 0)
    {
        Point T_R = (right_line[0]);
        Point B_R = (right_line[right_line.size() - 1]);
        Point center_begin_point = Point(B_R.x, B_R.y);
        Point center_end_point = Point(T_R.x, T_R.y);
        if (center_end_point.x - center_begin_point.x == 0)
            slop = 0;
        else
            slop = atan((center_begin_point.y - center_end_point.y) / (center_end_point.x - center_begin_point.x)) - PI / 2;
        if (!isPrinted)
        {
            isPrinted = true;
            // printf("b_c : %.2f, %.2f t_c : %.2f, %.2f\n", center_begin_point.x, center_begin_point.y, center_end_point.x, center_end_point.y);
            // printf("b_r : %.2f, %.2f t_r : %.2f, %.2f\n", B_R.x, B_R.y, T_R.x, T_R.y);
        }
        circle(src, center_begin_point, 10, Scalar(255, 0, 0), -1);
        circle(src, center_end_point, 10, Scalar(0, 255, 255), -1);

        vector<Point> pts;
        pts = {T_R, B_R};
        vector<vector<Point>> ppts;
        ppts.push_back(pts);

        line(src, Point(B_R), Point(T_R), Scalar(0, 255, 0), 10);
        line(src, Point(center_begin_point), Point(center_end_point), Scalar(0, 255, 255), 10);
    }
    else
    {
        slop = 0;
    }
}

int main()
{
    VideoCapture cap(1); // 鏡頭編號依序從 012...
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

        // img = imread(PATH);

        Mat original_image = img.clone();
        Mat yellow_line = img.clone();
        Mat white_line = img.clone();
        // GetROI(original_image, img);
        FiltGraph(original_image, yellow_line, 'y');
        FiltGraph(original_image, white_line, 'w');
        DetectRoadLine(original_image, yellow_line, white_line);
        imshow("frame", original_image);
        cout << slop << endl;
        if (waitKey(1) == 'q')
            break;
    }
    return 0;
}