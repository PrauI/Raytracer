#include "object.hpp"
#include <json/json.h>
#include <iostream>
#include <cmath>

using std::cout, std::endl;

void Object::setPosition(Json::Value& input){
    if(input.isArray() && input.size() == 3){
        position[0] = input[0].asInt();
        position[1] = input[1].asInt();
        position[2] = input[2].asInt();
        position[3] = 1;
    }else{
            // error weil das muss halt so sein. sonst halt falsch dicka
    }
}

Sphere::Sphere(Json::Value& input){
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
}
Vec4f Object::getPosition(){ return position;}

Vec3b Sphere::intersection(Vec4f& S, Vec4f& d){
    Vec4f C = getPosition();
    Vec4f SC = C - S;
    float scalarProd = scalarProduct(d, SC);
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

    return Vec3b(255, 0, 255);

}

float length(Vec4f& x){
    return sqrt(pow(x[0],2) + pow(x[1], 2) + pow(x[2], 2) + pow(x[3], 2));
}

float scalarProduct(Vec4f& a, Vec4f& b){
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2] + a[3]*b[3];
}