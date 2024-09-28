#include "object.hpp"
#include "world.hpp"
#include "ray.hpp"
#include <json/json.h>
#include <iostream>
#include <cmath>

using std::cout, std::endl, cv::Mat;


void Object::setPosition(Json::Value& input){
    if(input.isArray() && input.size() == 3){
        position[0] = input[0].asFloat();
        position[1] = input[1].asFloat();
        position[2] = input[2].asFloat();
        position[3] = 1;
    }else{
            // error weil das muss halt so sein. sonst halt falsch dicka
    }
}

void Object::setDefaultPosition(){
    position[0] = 0; position[1] = 0; position[2] = 0; position[3] = 1;
}



Sphere::Sphere(Json::Value& input, Mat& matrix){

    // set position
    try
    {
        if(!input.isMember("position")) throw std::runtime_error("No Position provided for Sphere");
        Json::Value inputPosition = input["position"];
        if(!inputPosition.isArray() || inputPosition.size() != 3) throw std::runtime_error("No correct Format given for Sphere Position");
        setPosition(inputPosition);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "Proceeding with Position 0,0,0" << endl;
        setDefaultPosition();
    }
    
    // set radius
    try
    {
       if(!input.isMember("radius")) throw std::runtime_error("No Radius provided for Sphere");
       radius = input["radius"].asFloat();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "Proceeding with radius 1.0" << endl;
        radius = 1.0;
    }
    
    // set color
    try{
        if(!input.isMember("color")) throw std::runtime_error("No Color provided for Sphere");
        Json::Value inputColor = input["color"];
        setColor(inputColor);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with default colors" << endl;
        setDefaultColor();
    }

    // set Index
    try{
        if(!input.isMember("index")) throw std::runtime_error("No Index provided for Sphere");
        Json::Value Jindex = input["index"];
        setIndex(Jindex);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with Index: 1" << endl;
    }

    transformationMatrix = matrix.inv();
    normalizeTransformationMatrix(transformationMatrix);
}
Vec4f Object::getPosition(){ return position;}
Vec3f Object::getAmbient(){ return ambient;}
Vec3f Object::getDiffuse(){ return diffuse;}
float Object::getShininess() {return shininess;}
Vec3f Object::getSpecular(){ return specular;}
Vec3f Object::getReflected(){ return reflected; }
Vec3f Object::getRefracted(){ return refracted; }
float Object::getIndex(){ return index; }

void Object::setDefaultColor() {
    ambient = Vec3f(1.0, 1.0, 1.0);
    diffuse = Vec3f(1.0, 1.0, 1.0);
    specular = Vec3f(1.0, 1.0, 1.0);
    reflected = Vec3f(1.0, 1.0, 1.0);
    refracted = Vec3f(1.0, 1.0, 1.0);
    shininess = 0.0;
}

void Object::setColor(Json::Value& color){
    // todo try catch

    for(int i = 0; i < 3; i++){
        ambient[i] = color["ambient"][i].asFloat();
        diffuse[i] = color["diffuse"][i].asFloat();
        specular[i] = color["specular"][i].asFloat();
        reflected[i] = color["reflected"][i].asFloat();
        refracted[i] = color["refracted"][i].asFloat();
    }
    shininess = color["shininess"].asFloat();
}

void Object::setIndex(Json::Value& Jindex){
    try{
        index = Jindex.asFloat();
    }catch(const std::exception& e){
        cout << e.what() << endl;
        index = 1.0;
        cout << "Proceeding with Index: 1" << endl;
    }
}

void Sphere::intersection(const struct Ray& ray, World* scene, intersectionInfo* closesHit){
    Mat resultD = transformationMatrix * ray.dir;
    Vec4f dm = resultD;

    Vec4f C = getPosition();

    Mat resultS = transformationMatrix * ray.position;
    Vec4f Sm = resultS;

    Vec4f L = C - Sm;
    float tca = L.dot(dm);
    if(tca < 0) return; // sphere is behind camera

    float d2 = L.dot(L) - tca * tca;
    if(d2 > radius * radius) return; // ray misses sphere

    float thc = sqrt(radius * radius - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;

    if(t0 > t1) std::swap(t0,t1);
    if(t0 < 0) {
        t0 = t1;
        if(t0 < 0) return;
    }

    if(t0 >= closesHit->t) return;
    closesHit->t = t0;
    closesHit->didHit = true;
    closesHit->object = this;
    closesHit->dir = -dm;

    closesHit->position = Sm + dm * t0;
    closesHit->normal = (closesHit->position - C) / radius;

    // cout << "intersection: " << value << endl;
    return;

}

bool Sphere::isIncluded(const Vec4f &point) {
    return pow(point[0] - position[0], 2) + pow(point[1] - position[1], 2) + pow(point[2] - position[2], 2) <= pow(radius, 2) + 0.0001;
}

float length(Vec4f& x){
    return sqrt(pow(x[0],2) + pow(x[1], 2) + pow(x[2], 2) + pow(x[3], 2));
}


bool Halfspace::isIncluded(const Vec4f &point) {
    Vec4f dirToPos = point - getPosition();
    return dirToPos.dot(normal) == 0;
}


void Halfspace::setNormal(Json::Value& input){
    // we already checked if input is in the right format beore calling the function
    for(int i = 0; i < 3; i++){
        normal[i] = input[i].asFloat();
    }
    normal[3] = 0;
    cv::normalize(normal, normal);
}

void Halfspace::setDefaultNormal(){
    normal = Vec4f(0.0);
}

Halfspace::Halfspace(Json::Value& input, Mat& matrix){
    // set position
    try{
        if(!input.isMember("position")) throw std::runtime_error("No Position provided for Halfspace");
        Json::Value inputPosition = input["position"];
        if(!inputPosition.isArray() || inputPosition.size() != 3) throw std::runtime_error("No correct Format given for Halfspace Positon");
        setPosition(inputPosition);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with position 0,0,0" << endl;
        setDefaultPosition();
    }
    // set normal
    try{
        if(!input.isMember("normal")) throw std::runtime_error("No Normal provided for Halfspace");
        Json::Value inputNormal = input["normal"];
        if(!inputNormal.isArray() || inputNormal.size() != 3) throw std::runtime_error("No correct Format given for Halfspace Normal");
        setNormal(inputNormal);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with normal 1,0,0" << endl;
        setDefaultNormal();
    }

    // set color
    try{
        if(!input.isMember("color")) throw std::runtime_error("No Color provided for Halfspace");
        Json::Value inputColor = input["color"];
        setColor(inputColor);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with default colors" << endl;
        setDefaultColor();
    }

     // set Index
    try{
        if(!input.isMember("index")) throw std::runtime_error("No Index provided for Halfspace");
        Json::Value Jindex = input["index"];
        setIndex(Jindex);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with Index: 1" << endl;
        index = 1;
    }

    transformationMatrix = matrix.inv();
    normalizeTransformationMatrix(transformationMatrix);
}

void Halfspace::intersection(const struct Ray& ray, World* scene, intersectionInfo* closesHit){
    Mat resultD = transformationMatrix * ray.dir;
    Vec4f dm = resultD;
    Mat resultS = transformationMatrix * ray.position;
    Vec4f Sm = resultS;
 
   // since a halfspace is an infinite plane the there is either an intersection point or the ray coming from the camera is parallel to the plane
   float scalarValue = this->normal.dot(dm);
   // cout << "normal: " << normal << " d: " << dm << " scalar: " << scalarValue << endl;
   if(scalarValue > -0.0001) return; // they are parallel and there is no intersection
   Vec4f pos = getPosition();

   float t = (pos - Sm).dot(normal) / scalarValue;
    if(t < 0) return; // intersection Point behind camera


    closesHit->t = t;
    closesHit->dir = -dm;
    closesHit->didHit = true;

    Vec4f intersectionPoint = Sm + t * dm;
    closesHit->position = intersectionPoint;
    closesHit->object = this;
    closesHit->normal = normal;

    return;
}

void normalizeTransformationMatrix(cv::Mat& transformationMatrix) {
    // Beispiel - Richtungsvektor
    Vec4f exampleDir = Vec4f(1,0,0,0);
    Mat transformationMat = transformationMatrix * exampleDir;
    Vec4f transformationDir = transformationMat;

    // calculate length of transformed Vector
    float length = std::sqrt(transformationDir.dot(transformationDir));
    transformationMatrix /= length;
}

Vec4f Sphere::getNormal(const Vec4f &position) {
    return (position - getPosition()) / radius;
}

Vec4f Halfspace::getNormal(const Vec4f &position) {
    (void) position;
    return normal;
}

CombinationWrapper::CombinationWrapper(Combination *combination) : combination(combination) {
        setDefaultPosition();
        setDefaultColor();
}

Vec4f CombinationWrapper::getNormal(const Vec4f &position) {
    // todo check what object has been at position and return its normal
    return Vec4f(0.0);
}




void CombinationWrapper::intersection(const struct Ray& ray, World* scene, intersectionInfo* closesHit) {
    if(combination != nullptr) combination->intersection(ray, scene, closesHit);
    else {
        std::cout << "No combination set" << std::endl;
        closesHit->didHit = false;
    }
}

bool CombinationWrapper::isIncluded(const Vec4f &point) {
    // todo !!
    (void) point;
    return false;
}



