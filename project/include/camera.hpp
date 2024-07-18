#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <opencv2/opencv.hpp>
#include <string>
#include <json/json.h>

using cv::Vec4f, cv::Vec3f, cv::Mat, std::string;

class Camera
{
private:
    Vec4f observerPosition;
    Vec4f screenPosition;
    Vec4f screen;
    int dpi;

public:
    Mat matrix;

    Camera(Vec4f& observerPosition, Vec4f& P, Vec4f& screen, int dpi);
    Camera(Json::Value& screen);
    Camera();
    void capture(const string& filename);
    Vec4f getPosition();
    Vec4f getObserver();
    int getDpi();
};

#endif