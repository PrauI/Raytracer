#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <opencv2/opencv.hpp>
#include <json/json.h>
#include "object.hpp"

using cv::Vec3f, cv::Vec4f;

class Object;
class Light{
private:

public:
    bool isAmbient;
    Vec3f intensity;
    virtual Vec3f lightValue(struct intersectionInfo* info) = 0;
};

class Ambient: public Light{
private:

public:
    Ambient(Json::Value& input);
    Vec3f lightValue(struct intersectionInfo* info);

};

class Source: public Light{
private:
    Vec4f position;
public:
    Source(Json::Value& input);
    Vec3f lightValue(struct intersectionInfo* info);
    Vec4f getPosition();
};

#endif