#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    // 创建相机对象
    cv::VideoCapture cap(1);

    // 检查相机是否成功打开
    if (!cap.isOpened())
    {
        std::cerr << "Error: Failed to open camera" << std::endl;
        return -1;
    }

    // 读取图像帧
    cv::Mat frame;
    // cap >> frame;
    bool ret = cap.read(frame);

    // 检查图像是否成功读取
    if (frame.empty())
    {
        std::cerr << "Error: Failed to capture frame" << std::endl;
        return -1;
    }

    // 保存图像到文件
    std::string filename = "captured_image.jpg";
    bool success = cv::imwrite(filename, frame);

    // 检查是否成功保存图像
    if (!success)
    {
        std::cerr << "Error: Failed to save image" << std::endl;
        return -1;
    }

    std::cout << "Image saved as " << filename << std::endl;

    // 释放相机对象
    cap.release();

    return 0;
}