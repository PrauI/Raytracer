#include "object.hpp"
#include "world.hpp"
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
        // todo defaultColor
    }

    transformationMatrix = matrix;
}
Vec4f Object::getPosition(){ return position;}
Vec3f Object::getAmbient(){ return ambient;}
Vec3f Object::getDiffuse(){ return diffuse;}
float Object::getShininess() {return shininess;}
Vec3f Object::getSpecular(){ return specular;}

void Object::setColor(Json::Value& color){
    // todo try catch

    for(int i = 0; i < 3; i++){
        ambient[i] = color["ambient"][i].asInt();
        diffuse[i] = color["diffuse"][i].asInt();
        specular[i] = color["specular"][i].asInt();
        reflected[i] = color["reflected"][i].asInt();
        refracted[i] = color["refracted"][i].asInt();
    }
    shininess = color["shininess"].asFloat();
}

struct intersectionInfo* Sphere::intersection(const Vec4f& S, const Vec4f& d, World* scene){
    intersectionInfo* result = new intersectionInfo;
    result->didHit = false;
    result->object = this;

    Vec4f C = getPosition();
    Mat resultD = transformationMatrix.inv() * d;
    Vec4f dm = resultD;
    cv::normalize(dm, dm);
    result->dir = -dm;
    // dm = dm * -1;
    Mat resultS = transformationMatrix.inv() * S;
    Vec4f Sm = resultS;

    Vec4f offsetRayOrigin = Sm - C;
    float a = dm.dot(dm);
    float b = 2 * offsetRayOrigin.dot(dm);
    float c = offsetRayOrigin.dot(offsetRayOrigin) - radius * radius;

    float discriminant = b * b - 4 * a * c;

    // no solution when d < 0 (ray misses sphere)
    if(discriminant < 0) return result;
    // else:
    result->didHit = true;

    float t = (-b - sqrt(discriminant)) / (2 * a);
    result->t = t;

    Vec4f intersectionPoint = Sm + dm * t;
    result->position = intersectionPoint;
    Vec4f normal = intersectionPoint - C;
    cv::normalize(normal, normal);
    result->normal = normal;

    // cout << "intersection: " << value << endl;
    return result;

}

float length(Vec4f& x){
    return sqrt(pow(x[0],2) + pow(x[1], 2) + pow(x[2], 2) + pow(x[3], 2));
}

float scalarProduct(const Vec4f& a, const Vec4f& b){
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
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
    for(int i = 0; i < 3; i++) normal[i] = 0;
    normal[0] = 1;
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
        // todo defaultColor
    }

    transformationMatrix = matrix;
}

struct intersectionInfo* Halfspace::intersection(const Vec4f& S, const Vec4f& d, World* scene){
    struct intersectionInfo* result = new intersectionInfo;
    result->didHit = false;
    result->object = this;

    Mat resultD = transformationMatrix.inv() * d;
    Vec4f dm = resultD;
    cv::normalize(dm, dm);
    result->dir = -dm;
    Mat resultS = transformationMatrix.inv() * S;
    Vec4f Sm = resultS;
 
   // since a halfspace is an infinite plane the there is either an intersection point or the ray coming from the camera is parallel to the plane
   float scalarValue = this->normal.dot(dm);
   // cout << "normal: " << normal << " d: " << dm << " scalar: " << scalarValue << endl;
   if(scalarValue > -0.0001) return result; // they are parallel and there is no intersection
   Vec4f pos = getPosition();

   float t = (pos-Sm).dot(normal) / scalarValue;
    if(t < 0) return result; // intersection Point behind camera
    if(t > 80) return result; // todo maybe change or delete

    result->t = t;
    result->didHit = true;

    Vec4f intersectionPoint = Sm + t * dm;
    result->position = intersectionPoint;

    result->normal = normal;

    return result;    
}