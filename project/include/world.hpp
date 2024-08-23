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
struct intersectionInfo;

class World {
    Camera camera;
    std::vector<Light*> lightList;
    std::vector<Object*> objectList;
public:
    World();
    void readFile(const string& filename);
    void createImage(const string& filename);
    void readObjects(Json::Value& objects);
    void setupObjects(Json::Value& object, Mat& matrix);
    void calcMatrix();
    intersectionInfo* hit(Vec4f& start, Vec4f& dir, Object* startingObject);
    Vec3f mixLight(struct intersectionInfo* info, int currentBounce, int maxBounce);
};

Vec3b map255(const Vec3f& color);
Vec3f addLight(Vec3f& color1, Vec3f& color2);
Vec3f skyColor(Vec4f& d);

#endif // WORLD_HPP
