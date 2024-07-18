#include <json/json.h>
#include <opencv2/opencv.hpp>
#ifndef OBJECT_HPP
#define OBJECT_HPP

using cv::Vec4f, cv::Vec3b, cv::Vec3f;

class Object{
private:
    Vec4f position;
    Vec3f ambient;
    Vec3f diffuse;
    Vec3f specular;
    Vec3f reflected;
    Vec3f refracted;
    int shininess;


public:
    void setPosition(Json::Value& input);
    void setColor(Json::Value& color);
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