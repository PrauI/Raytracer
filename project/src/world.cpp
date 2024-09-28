#include "world.hpp"
#include "object.hpp"
#include "light.hpp"
#include "ray.hpp"
#include <iostream>
#include <json/json.h>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <limits>
#include <thread>
#include <combination.hpp>
#define INFINITY std::numeric_limits<float>::infinity()

using std::cout, std::endl, cv::Mat;
using std::string;

World::World(){}

void World::readFile(const string& filename) {
    Json::Value scene;
    try {
        std::ifstream input_file(filename, std::ifstream::binary);
        if(!input_file.is_open()) throw std::runtime_error("Unable to open file");
        input_file >> scene;
    } catch (const std::exception& e) {
        std::cerr << e.what() << ": " << filename << std::endl;
        return;
    }

    // setup camera
    try {
        Json::Value screen;
        if(!scene.isMember(("screen"))) throw std::runtime_error("No screen provided in file");
        screen = scene["screen"];
        camera = Camera(screen);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cout << "using default camera settings" << std::endl;
        camera = Camera();

    }

    // setup Lights
    try {
        Json::Value medium;
        if(!scene.isMember("medium")) throw std::runtime_error("No medium light source provided in file");
        medium = scene["medium"];
        lightList.push_back(new Ambient(medium));
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cout << "using default ambient light settings" << std::endl;
        lightList.push_back(new Ambient());
    }

    // setup sources
    try {
        if(!scene.isMember(("sources"))) throw std::runtime_error("No light sources provided in file");
        Json::Value sources = scene["sources"];
        if(!sources.isArray()) throw std::runtime_error("Light Sources not in correct format");
        cout << "Light Sources: " << endl;
        for(auto source : sources) {
            lightList.push_back(new Source(source));
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cout << "Careful! No lights make it hart to see in the dark :(" << std::endl << "Continuing only with Ambient light" << std::endl;
        lightList = {};
    }


    // setup objects
    try {
        if(!scene.isMember("objects")) throw std::runtime_error("No objects provided in file");
        Json::Value objects;
        objects = scene["objects"];
        readObjects(objects);
    }catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cout << "Whats the point of raytracing, when there is nothing to see :(" << std::endl;
        objectList = {};
    }
}

void World::readObjects(Json::Value& objects){
    try {
        if(!objects.isArray()) throw std::runtime_error("Objects not in correct format");
        if(objects.size() == 0) throw std::runtime_error("No objects provided");
        for(auto object : objects) {

            // iterating over Objects
            Mat matrix = (cv::Mat_<float>(4,4) <<
                                            1.0, 0.0, 0.0, 0.0,
                                            0.0, 1.0, 0.0, 0.0,
                                            0.0, 0.0, 1.0, 0.0,
                                            0.0, 0.0, 0.0, 1.0);
            // for every transformation done to an object we go deeper in the json file,
            // until we get to the subject, in which case we break out of the loop.
            objectList.push_back(setupObjects(object, matrix));
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cout << "No objects in scene. Boring!" << std::endl;
        objectList = {};
    }
}

Object* World::setupObjects(Json::Value& object, Mat& matrix){
    Object* finalObject = nullptr;
    // da muss noch irgendwie die matrix mit rein
    if(object.isMember("sphere")) {
        finalObject = new Sphere(object["sphere"], matrix);
        return finalObject;
    }
    else if(object.isMember("halfSpace")){
        finalObject = new Halfspace(object["halfSpace"], matrix);
        return finalObject;
    }
    else if(object.isMember("union")) {
        Json::Value junion = object["union"];
        try {
            auto* unionObject = new Union();
            if(!junion.isArray()) throw std::runtime_error("Union not in correct format");
            if(junion.size() == 0) throw std::runtime_error("No objects in Union");
            for(int i = 0; i < junion.size(); i++) {
                unionObject->addObject(setupObjects(junion[i], matrix));
            }
            finalObject = new CombinationWrapper(unionObject);

            return finalObject;

        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            std::cout << "Rendering proceeds without union" << std::endl;
        }
    }
    else if(object.isMember("intersection")) {
        Json::Value jintersection = object["intersection"];
        try {
            auto* intersectionObject = new Intersection();
            if(!jintersection.isArray()) throw std::runtime_error("Intersection not in correct format");
            if(jintersection.size() == 0) throw std::runtime_error("No objects in intersection");
            for(int i = 0; i < jintersection.size(); i++) {
                intersectionObject->addObject(setupObjects(jintersection[i], matrix));
            }
            finalObject = new CombinationWrapper(intersectionObject);

            return finalObject;

        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            std::cout << "Rendering proceeds without intersection" << std::endl;
        }
    }
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
        return setupObjects(scaling["subject"], matrix);
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
        return setupObjects(translation["subject"], matrix);
    }
    else if(object.isMember("rotation")) {
        Json::Value rotation = object["rotation"];
        try {
            if(!rotation.isMember("factors")) throw std::runtime_error("No Factors provided for Rotation");
            Json::Value factors = rotation["factors"];
            if(!factors.isArray() || factors.size() != 3) throw std::runtime_error("Provided Factors for Rotation are not in the correct Format");
            float angleX = factors[0].asFloat();
            float angleY = factors[1].asFloat();
            float angleZ = factors[2].asFloat();
            Mat rotationMatrixX = (cv::Mat_<float>(4,4) <<
                                            1.0, 0.0, 0.0, 0.0,
                                            0.0, cos(angleX), -sin(angleX), 0.0,
                                            0.0, sin(angleX), cos(angleX), 0.0,
                                            0.0, 0.0, 0.0, 1.0);
            Mat rotatrionMatrixY = (cv::Mat_<float>(4,4) <<
                                            cos(angleY), 0.0, sin(angleY), 0.0,
                                            0.0, 1.0, 0.0, 0.0,
                                            -sin(angleY), 0.0, cos(angleY), 0.0,
                                            0.0, 0.0, 0.0, 1.0);
            Mat rotationMatrixZ = (cv::Mat_<float>(4,4) <<
                                            cos(angleZ), -sin(angleZ), 0.0, 0.0,
                                            sin(angleZ), cos(angleZ), 0.0, 0.0,
                                            0.0, 0.0, 1.0, 0.0,
                                            0.0, 0.0, 0.0, 1.0);
            matrix = rotationMatrixX * rotatrionMatrixY * rotationMatrixZ * matrix;

        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            std::cout << "Rendering proceeds without rotation" << std::endl;
        }

        if(!rotation.isMember("subject")) throw std::runtime_error("No subject Provided for Rotation");
        return setupObjects(rotation["subject"], matrix);
    }

}


void World::createImage(const string& filename){
    camera.capture(filename);
}

void World::hit(struct Ray& ray, Object* startingObject, intersectionInfo* closestHit){
    for(auto object : objectList){
        if(object == startingObject) continue;
        object->intersection(ray, this, closestHit);
    }
}

void World::processMatrix(int numThreads) {
    int rows = camera.matrix.rows;
    int cols = camera.matrix.cols;
    int blockSize = rows / numThreads;

    std::vector<std::thread> threads;

    for(int i = 0; i < numThreads; ++i) {
        int startRow = i * blockSize;
        int endRow = (i == numThreads - 1) ? rows : (i + 1) * blockSize;
        threads.emplace_back([this, startRow, endRow, cols]() {
            this->calcMatrix(startRow, endRow, 0, cols);
        });
    }

    for(auto& thread : threads) {
        thread.join();
    }

    // flip the image
    cv::Mat flipped = std::move(camera.matrix);
    cv::flip(flipped, camera.matrix, 0);
}

void World::calcMatrix(int startRow, int endRow, int startCol, int endCol){
    struct intersectionInfo closestHit; // changing to stack memory
    float dpi = camera.getDpi();
    Vec4f cameraPosition = camera.getPosition();
    Vec4f cameraObserver = camera.getObserver();
    for(int y = startRow; y < endRow; ++y){
        Vec3b* row_ptr = camera.matrix.ptr<Vec3b>(y);  // Zeiger auf die Zeile
    for(int x = startCol; x < endCol; ++x){
        closestHit = {.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr};
        Vec4f delta {float(y) / dpi, float(x) / dpi,0,0};
        Vec4f S = cameraPosition + delta;
        Vec4f d = S - cameraObserver;
        cv::normalize(d,d);
        Ray ray = Ray{.dir = std::move(d), .position = std::move(S)};

        hit(ray, nullptr, &closestHit); // nullptr weil wir die kamera sind

        Vec3f color = {1.0,0.7,0.5};
        if(closestHit.didHit) color = mixLight(&closestHit, 0, 2);

        else color = skyColor(ray.dir);
        row_ptr[x] = map255(color);
    }
    }

}

Vec3f skyColor(const Vec4f& d){
    float a = 0.5 * (d[0] + 1.0);
    return (1.0 - a) * Vec3f{1.0, 1.0, 1.0} + a * Vec3f{1.0, 0.7, 0.5};
}

// maped zahlen aus dem raum 0-1 auf 0-255
Vec3b map255(const Vec3f& color){
    Vec3b rgb;
    for(int i = 0; i < 3; i++){
        rgb[i] = static_cast<unsigned char>(color[i] * 255);
    }
    return rgb;
}

// Calculates Color / Light at a certain intersection point 
// based on all the lights in the scene
Vec3f World::mixLight(struct intersectionInfo* info, int currentBounce, int maxBounce){
    // Vec3f color = lightList[0]->lightValue(V, P, N, object);
    Vec3f color = {0,0,0};
    for(auto light : lightList){

        // calculate if intersection point lays in shadow
        // calculate if ray between object and light is blocked by an object
        if(!light->isAmbient){
            bool inShadow = false;

            Source* source = dynamic_cast<Source*>(light); // Light class muss in Source class umgewandelt werden, weil nur source eine getPosition funktion hat

            Object* hitObject = nullptr;
            inShadow = isBlocked(info->position, source->getPosition(), this, hitObject);
            if(!inShadow) {
                Vec3f incomingColor = light->lightValue(info);
                color = addLight(color, incomingColor);
            }else if (inShadow && hitObject->getRefracted() != Vec3f(0.0)) {
                Vec3f incomingColor = light->lightValue(info) * 0.5;
                color = addLight(incomingColor, color);
            }
        }

        if(light->isAmbient){
            Vec3f incomingColor = light->lightValue(info);
            color = addLight(color, incomingColor);
        }
        // calculating the reflected light
        if(currentBounce < maxBounce) {
            // reflected
            float scalar = info->dir.dot(info->normal);
            Vec4f R = info->dir - 2*(scalar) * info->normal;
            cv::normalize(R,R);
            R = R * -1;
            struct Ray reflectedRay = {.dir = R, .position = info->position};
            intersectionInfo reflectedHit = {.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr};
            hit(reflectedRay, info->object, &reflectedHit);
            Vec3f rcolor;
            Vec3f Kr = info->object->getReflected();
            if(reflectedHit.didHit && reflectedHit.t > 0){
                rcolor = mixLight(&reflectedHit, currentBounce + 1, maxBounce);
                for(int i = 0; i < 3; i++){ rcolor[i] = rcolor[i] * Kr[i] * 0.5; }
            }else {
                Vec3f sky = skyColor(reflectedRay.dir);
                for(int i = 0; i < 3 ; i++) rcolor[i] = sky[i] * Kr[i] * 0.5;
            }
            color = addLight(color, rcolor);

            // refracted
            if(info->object->getRefracted() != Vec3f(0.0)){
                float n = 1.0;
                float nt = info->object->getIndex();
                Vec4f T = refractedDir(n, nt, info->dir, info->normal);
                struct Ray refractedRay = finalRefractedRay(T, info->position, info->object);

                intersectionInfo refractedHit = {.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr};
                hit(refractedRay, info->object, &refractedHit);
                Vec3f tcolor;
                Vec3f Kt = info->object->getRefracted();
                if(refractedHit.didHit && refractedHit.t > 0){
                    tcolor = mixLight(&refractedHit, currentBounce + 1, maxBounce);
                    for(int i = 0; i < 3; i++){ tcolor[i] = tcolor[i] * Kt[i] * 0.5; }
                }else{
                    Vec3f sky = skyColor(refractedRay.dir);
                    for(int i = 0; i < 3 ; i++) tcolor[i] = sky[i] * Kt[i] * 0.5;
                }
                color = addLight(color, tcolor);
        }
        }
        
        // color += incomingColor;
    }
    for(int i = 0; i < 3; i++) if(color[i] > 1) color[i] = 1;
                //     color[0] = info->normal[0];
                // color[1] = info->normal[1];
                // color[2] = info->normal[2];
    
    return color;
}

bool isBlocked(const Vec4f& startPoint, const Vec4f& lightPos, World* scene, Object*& hitObject) {
    Vec4f dir = lightPos - startPoint;
    float dist = sqrt(dir.dot(dir));
    cv::normalize(dir, dir);
    struct Ray lightRay = {.dir = dir, .position = startPoint + 0.00001 * dir}; /// + 0.0001 + dir ensures we are not colliding wiht the starting point
    intersectionInfo shadowHit = {.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr};
    scene->hit(lightRay, nullptr, &shadowHit);
    hitObject = shadowHit.object;
    if(shadowHit.didHit &&shadowHit.t < dist && shadowHit.t > 0) return true;
    return false;
}

Vec4f refractedDir(const float n, const float nt, const Vec4f& dir, const Vec4f& normal) {
    const float refractiveIndex = n / nt;
    float scalar = dir.dot(normal);
    Vec4f a = (scalar + sqrt(pow( 1 /refractiveIndex, 2) + pow(scalar, 2) - 1)) * normal;
    Vec4f T = dir - a;
    T *= refractiveIndex;
    cv::normalize(T, T);
    return T;
}

struct Ray finalRefractedRay(Vec4f& T, Vec4f& enterPoint, Object* object){
    intersectionInfo refractedHit = {.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr};
    intersectionInfo selfCollision = {.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr};
    struct Ray refractedRay = {.dir = T, .position = enterPoint + 0.00001 * T}; // multiplication to avoid intersection with startingpoint

    // entering the object to calculate where it leaves
    object->intersection(refractedRay, nullptr, &selfCollision); // we have to use nullptr because we want the collision on the other side of the object (if it has two sides)
    if(!selfCollision.didHit) return refractedRay; // it allready left the object / the object has no opposite side

    // it left object on the other side
    Vec4f Tn = refractedDir(object->getIndex(), 1.0, refractedRay.dir, selfCollision.normal * -1);
    refractedRay = {.dir = Tn, .position = selfCollision.position};
    return refractedRay;
}

// Funktion zum addieren von Licht / Helligkeit
// Light intensity is percived logarithmicaly, addition works as follows:
// E = E1 + E2 = 1 - (1 - E1)(1 - E2)
Vec3f addLight(Vec3f& color1, Vec3f& color2){
    Vec3f finalColor = {0,0,0};
    for(int i = 0; i < 3; i++){
        finalColor[i] = color1[i] + (1.0 - color1[i]) * color2[i];
    }
    return finalColor;
}