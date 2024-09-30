#include "light.hpp"
#include "object.hpp"
#include "world.hpp"
#include <iostream>
#include <math.h>

using std::cout;
using std::endl;

Ambient::Ambient(Json::Value& medium){
    isAmbient = true;

    try {
        if(!medium.isMember("ambient")) throw std::runtime_error("No ambient provided in medium");
        Json::Value ambient = medium["ambient"];
        if (!ambient.isArray() || ambient.size() != 3) throw std::runtime_error("Invalid ambient provided in medium");
        intensity[0] = ambient[0].asFloat();
        intensity[1] = ambient[1].asFloat();
        intensity[2] = ambient[2].asFloat();
    } catch (const std::exception& e) {
        std::cerr << e.what() << endl;
        cout << "proceding without ambient 0.1, 0.1, 0.1" << endl;
        intensity = Vec3f(0.1,0.1,0.1);
    }
}

Ambient::Ambient(): Light(){};

Source::Source(Json::Value& input){
    isAmbient = false;
    try {
        if(!input.isMember("position")) throw std::runtime_error("No position provided in source");
        if(!input.isMember("intensity")) throw std::runtime_error("No intensity provided in source");

        Json::Value inputPos = input["position"];
        Json::Value inputIntensity = input["intensity"];

        if(!inputPos.isArray() || inputPos.size() != 3) throw std::runtime_error("Invalid position provided in source");
        if(!inputIntensity.isArray() || inputIntensity.size() != 3) throw std::runtime_error("Invalid intensity provided in source");

        for(int i = 0; i < 3; i++){
            intensity[i] = inputIntensity[i].asFloat();
            position[i]  = inputPos[i].asFloat();
        }
        position[3] = 1;


    } catch (const std::exception& e) {
        std::cerr << e.what() << endl;
        cout << "proceding without source light" << endl;
        position = Vec4f(0,0,0,1);
        intensity = Vec3f(0,0,0);
    }
}

Vec3f Ambient::lightValue(struct intersectionInfo* info){
    Vec3f Ea;
    Vec3f Ka = info->object->getAmbient();
    for(int i = 0; i < 3; i++){
        Ea[i] = intensity[i] * Ka[i];
    }
    /**
    if(info->object->hasTexture()) {
        Vec3f Et = info->object->getTextureColor(info->position);
        for(int i = 0; i < 3; i++) {
            Et[i] *= intensity[i];
        }
        return addLight(Ea, Et);
    }
    */
    return Ea;
}

Vec3f Source::lightValue(struct intersectionInfo* info){
    // Diffuse component
    Vec3f Ed;
    Vec3f Kd = info->object->getDiffuse();
    Vec4f L = position - info->position;
    cv::normalize(L,L);
    float scalar = L.dot(info->normal);
    if(scalar < 0) scalar = 0;

    // specular component
    Vec3f Es;
    Vec3f Ks = info->object->getSpecular();
    Vec4f R = L - 2* scalar * info->normal;
    cv::normalize(info->dir, info->dir);
    float scalar2 = R.dot(-info->dir);
    if(scalar2 < 0) scalar2 = 0;


     for(int i = 0; i < 3; i++){
        Ed[i] = Kd[i] * intensity[i] * scalar;
        Es[i] = Ks[i] * intensity[i] * pow(scalar2, info->object->getShininess());
    }

    return addLight(Ed, Es);

}

Vec4f Source::getPosition(){ return position; }