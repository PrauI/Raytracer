#include "world.hpp"
#include "object.hpp"
#include "light.hpp"
#include <iostream>
#include <json/json.h>
#include <fstream>
#include <opencv2/opencv.hpp>

using std::cout, std::endl;
using std::string;

World::World(){}

void World::readFile(const string& filename) {
    std::ifstream input_file(filename, std::ifstream::binary);
    Json::Value scene;
    input_file >> scene;

    // setup camera
    Json::Value screen;
    screen = scene["screen"];
    camera = Camera(screen);

    // setup Lights
    Json::Value medium;
    medium = scene["medium"];
    lightList.push_back(new Ambient(medium));

    Json::Value sources = scene["sources"];
    cout << "Light Sources: " << endl;
    if(sources.isArray()){
        for(int i = 0; i < int(sources.size()); i++){
            lightList.push_back(new Source(sources[i]));
        }
    }else{
        // error handling..
    }

    // setup objects
    Json::Value objects;
    objects = scene["objects"];
    readObjects(objects);
}

void World::readObjects(Json::Value& objects){
    if(objects.isArray()){
        for(int i = 0; i < int(objects.size()); i++){
            // iterating over objects
            Object* object = nullptr;
            if(objects[i].isMember("sphere")){
                object = new Sphere(objects[i]["sphere"]);
            }
            
            if(object != nullptr) objectList.push_back(object);
        }
    }else{
        // fehler weil es soll ein array sein!
    }

    // if(objects.isMember("sphere")){
    //     Object* sphere = new Sphere(objects["sphere"]);
    // }else{
    //     cout << "no sphere" << endl;
    // }
}


void World::createImage(const string& filename){
    camera.capture(filename);
}

void World::calcMatrix(){
    for(int x = 0; x < camera.matrix.cols + 1; x++){
    for(int y = 0; y < camera.matrix.rows + 1; y++){
        Vec4f delta {float(y) / camera.getDpi(), float(x) / camera.getDpi(),0,0};
        Vec4f S = camera.getPosition() + delta;
        Vec4f d = S - camera.getObserver();
        cv::normalize(d,d);
        // cout << "S: " << S[0] << ", " << S[1] << ", " << S[2] << ", " << S[3];
        // cout << "d: " << d[0] << ", " << d[1] << ", " << d[2] << ", " << d[3] << endl;
        // for(Object* object : objectList){
        //     camera.matrix.at<cv::Scalar>(y,x) = object->intersection();
        // }
        Vec3f color = objectList[0]->intersection(S, d, this);
        // cout << color << endl;
        Vec3b rgbColor = map255(color);
        // cout << color << rgbColor << endl;
        camera.matrix.at<cv::Vec3b>(y,x) = rgbColor;;
    }
    }
    
}

Vec3b map255(const Vec3f& color){
    Vec3b rgb;
    for(int i = 0; i < 3; i++){
        rgb[i] = color[i] * 255;
    }
    return rgb;
}

Vec3f World::mixLight(const Vec4f& V, const Vec4f& P, const Vec4f& N, Object* object){
    Vec3f color = {0,0,0};
    for(auto light : lightList){
        color += light->lightValue(V, P, N, object);
    }
    return color;
}