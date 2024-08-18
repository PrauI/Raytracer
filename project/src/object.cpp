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
    if(input.isMember("position")){
        Json::Value inputPosition = input["position"];
        setPosition(inputPosition);
    }else{
        // error weil wir brauchen eine position
        cout << "No Position given for sphere" << endl;
    }

    if(input.isMember("radius")) radius = input["radius"].asFloat();
    else{
        // error weil wir brauchen einen radius
    }

    if(input.isMember("color")){
        Json::Value color = input["color"];
        setColor(color);

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

Vec3f Sphere::intersection(const Vec4f& S, const Vec4f& d, World* scene){
    Vec4f C = getPosition();
    Mat resultD = transformationMatrix.inv() * d;
    Vec4f dm = resultD;
    Mat resultS = transformationMatrix.inv() * S;
    Vec4f Sm = resultS;
    Vec4f SC = C - Sm;
    float scalarProd = scalarProduct(dm, SC);
    float SC_length = length(SC);
    float delta = pow(radius, 2) - pow(SC_length, 2) + pow(scalarProd, 2);
    
    if (delta < 0) {
        // Keine Schnittpunkte
        return Vec3b(0, 0, 0);
    }

    float sqrtDelta = sqrt(delta);
    float t1 = scalarProd + sqrtDelta;
    float t2 = scalarProd - sqrtDelta;
    float t;

    if (t1 >= 0 && t2 >= 0) {
        t = (t1 < t2) ? t1 : t2;
    } else if (t1 >= 0) {
        t = t1;
    } else if (t2 >= 0) {
        t = t2;
    } else {
        cout << t1 << t2 << endl;
        return Vec3b(0, 0, 0); // Beide t1 und t2 sind negativ
    }

    // calculate Position
    Vec4f intersectionPosition = Sm + t * dm;
    Vec4f normal = intersectionPosition - C;
    cv::normalize(normal, normal);
    Vec3f value = scene->mixLight(dm, intersectionPosition, normal, this);
    // cout << "intersection: " << value << endl;
    return value;

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

Vec3f Halfspace::intersection(const Vec4f& S, const Vec4f& d, World* scene){
    Mat resultD = transformationMatrix.inv() * d;
    Vec4f dm = resultD;
    cv::normalize(dm, dm);
    Mat resultS = transformationMatrix.inv() * S;
    Vec4f Sm = resultS;
 
   // since a halfspace is an infinite plane the there is either an intersection point or the ray coming from the camera is parallel to the plane
   float scalarValue = scalarProduct(normal, dm);
   // cout << "normal: " << normal << " d: " << dm << " scalar: " << scalarValue << endl;
   if(scalarValue < 0.0001) return Vec3b(0,0,0); // they are parallel and there is no intersection
   Vec4f pos = getPosition();

   float t = scalarProduct(pos - Sm, normal) / scalarValue;
    if(t > 0) return Vec3b(0,0,0); // intersection Point behind camera

    Vec4f intersectionPoint = Sm + t * dm;



    Vec3f value = scene->mixLight(-dm, intersectionPoint, normal, this);
    return value;    
}