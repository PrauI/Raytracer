#include <json/json.h>
#include <opencv2/opencv.hpp>
#ifndef OBJECT_HPP
#define OBJECT_HPP

using cv::Vec4f, cv::Vec3b;

class Object{
private:
    Vec4f position;


public:
    void setPosition(Json::Value& input);
    Vec4f getPosition();
    virtual Vec3b intersection(const Vec4f& S, const Vec4f& d) = 0;
};

class Sphere: public Object{
private:
    float radius;

public:
    Sphere(Json::Value& input);

    Vec3b intersection(const Vec4f& S, const Vec4f& d);
};

float length(Vec4f& x);
float scalarProduct(const Vec4f& a, const Vec4f& b);

#endif