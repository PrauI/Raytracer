#ifndef OBJECT_HPP
#define OBJECT_HPP
#include <json/json.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include "world.hpp"

using cv::Vec4f, cv::Vec3b, cv::Vec3f, cv::Mat;

class World;

#define Origin  Vec4f(0,0,0,1);
#define EPSILON 0.00001;

class Object;

struct intersectionInfo{
    bool didHit;
    float t;
    Vec4f position;
    Vec4f normal;
    Vec4f dir;
    Object* object;
};

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
    void setDefaultPosition();
    void setColor(Json::Value& color);
    Vec4f getPosition();
    Vec3f getAmbient();
    Vec3f getDiffuse();
    Vec3f getSpecular();
    float getShininess();
    virtual struct intersectionInfo* intersection(const Vec4f& S, const Vec4f& d, World* scene) = 0;
};

class Sphere: public Object{
private:
    float radius;

public:
    Sphere(Json::Value& input, Mat& matrix);

    struct intersectionInfo* intersection(const Vec4f& S, const Vec4f& d, World* scene);
};

class Halfspace: public Object{
private:
    Vec4f normal;

public:
    Halfspace(Json::Value& input, Mat& matrix);
    struct intersectionInfo* intersection(const Vec4f& S, const Vec4f& d, World* scene);
    void setNormal(Json::Value& input);
    void setDefaultNormal();
};

float length(Vec4f& x);
float scalarProduct(const Vec4f& a, const Vec4f& b);




#endif