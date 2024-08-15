#include "world.hpp"
#include "object.hpp"
#include "light.hpp"
#include <iostream>
#include <json/json.h>
#include <fstream>
#include <opencv2/opencv.hpp>

using std::cout, std::endl, cv::Mat;
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
            // iterating over Objects
            Mat matrix = (cv::Mat_<float>(4,4) << 
                                            1.0, 0.0, 0.0, 0.0,
                                            0.0, 1.0, 0.0, 0.0,
                                            0.0, 0.0, 1.0, 0.0,
                                            0.0, 0.0, 0.0, 1.0);
            // for every transformation done to an object we go deeper in the json file,
            // until we get to the subject, in which case we break out of the loop.
            setupObjects(objects[i], matrix);       
        }
    }else{
        // fehler weil es soll ein array sein!
    }
}

void World::setupObjects(Json::Value& object, Mat& matrix){
    Object* finalObject = nullptr;
    // da muss noch irgendwie die matrix mit rein
    if(object.isMember("sphere")) {
        finalObject = new Sphere(object["sphere"], matrix);
        objectList.push_back(finalObject);
    }
    else if(object.isMember("halfspace")) return; // to implement
    else if(object.isMember("union")) return; // to implement
    else if(object.isMember("scaling")) return; // to implement
    else if(object.isMember("translation")){
        Json::Value translation = object["translation"];
        try
        {
            if(!translation.isMember("factors")) throw std::runtime_error("No Factors provided for Translation");
            Json::Value factors = translation["factors"];
            if(!factors.isArray() || factors.size() != 3) throw std::runtime_error("Provided Factors for Translation are not in the correct Format");
            matrix.at<float>(0,3) = factors[0].asFloat();
            matrix.at<float>(1,3) = factors[1].asFloat();
            matrix.at<float>(2,3) = factors[2].asFloat();
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            std::cout << "Rendering proceeds without translation" << endl;
        }
        
        if(!translation.isMember("subject")) throw std::runtime_error("No subject Provided for Translation");
        setupObjects(translation["subject"], matrix);
    }
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
        camera.matrix.at<cv::Vec3b>(y,x) = map255(color);
    }
    }
    
}

// maped zahlen aus dem raum 0-1 auf 0-255
Vec3b map255(const Vec3f& color){
    Vec3b rgb;
    for(int i = 0; i < 3; i++){
        rgb[i] = color[i] * 255;
    }
    return rgb;
}

// Calculates Color / Light at a certain intersection point 
// based on all the lights in the scene
Vec3f World::mixLight(const Vec4f& V, const Vec4f& P, const Vec4f& N, Object* object){
    // Vec3f color = lightList[0]->lightValue(V, P, N, object);
    Vec3f color = {0,0,0};
    for(auto light : lightList){
        Vec3f incomingColor = light->lightValue(V, P, N, object);
        color = addLight(color, incomingColor);
        // color += incomingColor;
    }
    for(int i = 0; i < 3; i++) if(color[i] > 1) color[i] = 1;
    
    return color;
}

// Funktion zum addieren von Licht / Helligkeit
// Light intensity is percived logarithmicaly, addition works as follows:
// E = E1 + E2 = 1 - (1 - E1)(1 - E2)
Vec3f addLight(Vec3f& color1, Vec3f& color2){
    Vec3f finalColor = {0,0,0};
    for(int i = 0; i < 3; i++){
        finalColor[i] = 1.0 - (1.0 - color1[i]) * (1.0 - color2[i]);
    }
    return finalColor;
}