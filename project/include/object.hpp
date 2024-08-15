#ifndef OBJECT_HPP
#define OBJECT_HPP
#include <json/json.h>
#include <opencv2/opencv.hpp>
#include "world.hpp"

using cv::Vec4f, cv::Vec3b, cv::Vec3f, cv::Mat;

class World;

class Object{
private:
    Vec4f position;
    Vec3f ambient;
    Vec3f diffuse;
    Vec3f specular;
    Vec3f reflected;
    Vec3f refracted;
    float shininess;


public:
    Mat transformationMatrix;
    void setPosition(Json::Value& input);
    void setColor(Json::Value& color);
    Vec4f getPosition();
    Vec3f getAmbient();
    Vec3f getDiffuse();
    Vec3f getSpecular();
    float getShininess();
    virtual Vec3f intersection(const Vec4f& S, const Vec4f& d, World* scene) = 0;
};

class Sphere: public Object{
private:
    float radius;

public:
    Sphere(Json::Value& input, Mat& matrix);

    Vec3f intersection(const Vec4f& S, const Vec4f& d, World* scene);
};

float length(Vec4f& x);
float scalarProduct(const Vec4f& a, const Vec4f& b);


#endif