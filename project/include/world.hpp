#ifndef WORLD_HPP
#define WORLD_HPP

#include <json/json.h>
#include <opencv2/opencv.hpp>
#include <string>
#include "camera.hpp"
#include "light.hpp"
#include "object.hpp"
#include <vector>

using std::string, cv::Vec3f, cv::Vec4f, cv::Vec3b;

class Object;
class Light;

class World {
    Camera camera;
    std::vector<Light*> lightList;
    std::vector<Object*> objectList;
public:
    World();
    void readFile(const string& filename);
    void createImage(const string& filename);
    void readObjects(Json::Value& objects);
    void calcMatrix();
    Vec3f mixLight(const Vec4f& V, const Vec4f& P, const Vec4f& N, Object* object);
};

Vec3b map255(const Vec3f& color);

#endif // WORLD_HPP
