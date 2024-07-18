#ifndef LIGHT_HPP
#define LIGHT_HPP
#include <opencv2/opencv.hpp>
#include <json/json.h>

using cv::Vec3f, cv::Vec4f;

class Light{
private:

public:
    Vec3f intensity;
};

class Ambient: public Light{
private:

public:
    Ambient(Json::Value& input);

};

class Source: public Light{
private:
    Vec4f position;
public:
    Source(Json::Value& input);
};

#endif