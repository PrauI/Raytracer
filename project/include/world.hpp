#ifndef WORLD_HPP
#define WORLD_HPP

#include <json/json.h>
#include <string>
#include "camera.hpp"
#include "object.hpp"
#include <vector>
using std::string;

class World {
    Camera camera;
    std::vector<Object*> objectList;
public:
    World();
    void readFile(const string& filename);
    void createImage(const string& filename);
    void readObjects(Json::Value& objects);
    void calcMatrix();
};

#endif // WORLD_HPP
