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
        // todo in try catch umwandeln
    }
}

void World::setupObjects(Json::Value& object, Mat& matrix){
    Object* finalObject = nullptr;
    // da muss noch irgendwie die matrix mit rein
    if(object.isMember("sphere")) {
        finalObject = new Sphere(object["sphere"], matrix);
        objectList.push_back(finalObject);
    }
    else if(object.isMember("halfSpace")){
        finalObject = new Halfspace(object["halfSpace"], matrix);
        objectList.push_back(finalObject);
    }
    else if(object.isMember("union")) return; // todo
    else if(object.isMember("scaling")){
        Json::Value scaling = object["scaling"];
        try{
            if(!scaling.isMember("factors")) throw std::runtime_error("No Factors provided for Scaling");
            Json::Value factors = scaling["factors"];
            if(!factors.isArray() || factors.size() != 3) throw std::runtime_error("Provided Factors for Scaling  are not in the correct Format");
            Mat translationMatrix = (cv::Mat_<float>(4,4) << 
                                            factors[0].asFloat(), 0.0, 0.0, 0.0,
                                            0.0, factors[1].asFloat(), 0.0, 0.0,
                                            0.0, 0.0, factors[2].asFloat(), 0.0,
                                            0.0, 0.0, 0.0, 1.0);
            matrix = translationMatrix * matrix; 
        }catch(const std::exception& e){
            std::cerr << e.what() << endl;
            cout << "Rendering proceeds without scaling" << endl;
        }
        if(!scaling.isMember("subject")) throw std::runtime_error("No subject Provided for Scaling");
        setupObjects(scaling["subject"], matrix);
    }
    else if(object.isMember("translation")){
        Json::Value translation = object["translation"];
        try
        {
            if(!translation.isMember("factors")) throw std::runtime_error("No Factors provided for Translation");
            Json::Value factors = translation["factors"];
            if(!factors.isArray() || factors.size() != 3) throw std::runtime_error("Provided Factors for Translation are not in the correct Format");
            Mat translationMatrix = (cv::Mat_<float>(4,4) << 
                                            1.0, 0.0, 0.0, factors[0].asFloat(),
                                            0.0, 1.0, 0.0, factors[1].asFloat(),
                                            0.0, 0.0, 1.0, factors[2].asFloat(),
                                            0.0, 0.0, 0.0, 1.0);
            matrix = translationMatrix * matrix;
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

intersectionInfo* World::hit(Vec4f& start, Vec4f& dir, Object* startingObject){
    intersectionInfo* closestHit = nullptr;
    for(auto object : objectList){
        if(object == startingObject) continue;
        intersectionInfo* info = object->intersection(start, dir, this);
        if(info->didHit) {
            // überprüfe, ob dies der erste Treffer ist oder ob der aktuelle Treffer näher ist als der vorherige
            if(closestHit == nullptr || info->t < closestHit->t){
                delete closestHit;
                closestHit = info;  
                info = nullptr;    
            }else{
                delete info;
                info = nullptr;
            }
        }else{
            delete info;
            info = nullptr;
        } 
    }
    return closestHit; // be carefull. this could return a nullptr!!
}

void World::calcMatrix(){
    for(int x = 0; x < camera.matrix.cols + 1; x++){
    for(int y = 0; y < camera.matrix.rows + 1; y++){
        Vec4f delta {float(y) / camera.getDpi(), float(x) / camera.getDpi(),0,0};
        Vec4f S = camera.getPosition() + delta;
        Vec4f d = S - camera.getObserver();
        cv::normalize(d,d);

        struct intersectionInfo* closestHit = hit(S, d, nullptr); // nullptr weil wir die kamera sind

        Vec3f color = {1.0,0.7,0.5};
        if(closestHit != nullptr && closestHit->didHit) color = mixLight(closestHit, 0, closestHit->object->getIndex());
        else color = skyColor(d);
        delete closestHit;
        closestHit = nullptr;
        camera.matrix.at<cv::Vec3b>(camera.matrix.rows - y,x) = map255(color);
    }
    }
    
}

Vec3f skyColor(Vec4f& d){
    // float a = 0.5 * (d[0] + 1.0);
    // return (1.0 - a) * Vec3f{1.0, 0.7, 0.5} + a * Vec3f{1.0, 1.0, 1.0};
    return Vec3f{1.0, 0.7, 0.5};
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
Vec3f World::mixLight(struct intersectionInfo* info, int currentBounce, int maxBounce){
    // Vec3f color = lightList[0]->lightValue(V, P, N, object);
    Vec3f color = {0,0,0};
    for(auto light : lightList){

        // calculate if ray between object and light is blocked by an object
        bool inShadow = false;
        if(!light->isAmbient){

            Source* source = dynamic_cast<Source*>(light); // Light class muss in Source class umgewandelt werden, weil nur source eine getPosition funktion hat
            Vec4f dir = source->getPosition() - info->position;
            float dist = sqrt(scalarProduct(dir, dir));
            cv::normalize(dir, dir);
            intersectionInfo* shadowHit = hit(info->position, dir, info->object);
            if(shadowHit != nullptr && shadowHit->didHit && shadowHit->t < dist && shadowHit->t > 0) inShadow = true;
            delete shadowHit;
        }

        if(!inShadow){
            Vec3f incomingColor = light->lightValue(info);
            color = addLight(color, incomingColor);
        }
        if(light->isAmbient){
            Vec3f incomingColor = light->lightValue(info);
            color = addLight(color, incomingColor);
        }
        // calculating the reflected light
        if(currentBounce < maxBounce){
            Vec4f R = info->dir - 2*(scalarProduct(info->dir, info->normal)) * info->normal;
            cv::normalize(R,R);
            R = R * -1;
            intersectionInfo* reflectedHit = hit(info->position, R, info->object);
            Vec3f rcolor;
            if(reflectedHit != nullptr && reflectedHit->didHit && reflectedHit->t > 0){
                rcolor = mixLight(reflectedHit, currentBounce + 1, maxBounce);
                Vec3f Kr = info->object->getReflected();
                float scalar = scalarProduct(R, info->normal);
                for(int i = 0; i < 3; i++){ rcolor[i] = rcolor[i] * Kr[i] * scalar; }
            }else rcolor = {0,0,0};
            color = addLight(color, rcolor);
        }
        
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