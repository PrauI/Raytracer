#include "light.hpp"
#include "object.hpp"
#include "world.hpp"
#include <iostream>
#include <math.h>

using std::cout;
using std::endl;

Ambient::Ambient(Json::Value& medium){
    isAmbient = true;
    if(medium.isMember("ambient")){
        const Json::Value ambient = medium["ambient"];
        if(ambient.isArray() && ambient.size() == 3){
            intensity[0] = ambient[0].asFloat();
            intensity[1] = ambient[1].asFloat();
            intensity[2] = ambient[2].asFloat();

            cout << "Ambient Light: " << intensity[0] << " " << intensity[1] << " " << intensity[2] << endl;
        }else{
            // default werte und fehler behandlung
        }

    }else{
        // irgend einen default setten und ausgabe machen
    }
}

Ambient::Ambient(): Light(){};

Source::Source(Json::Value& input){
    isAmbient = false;
    if(input.isMember("position") && input.isMember("intensity")){
        Json::Value inputPos = input["position"];
        Json::Value inputIntensity = input["intensity"];

        for(int i = 0; i < 3; i++){
            intensity[i] = inputIntensity[i].asFloat();
            position[i]  = inputPos[i].asFloat();
        }
        position[3] = 1;

        cout << "Position: " << position[0] << " " << position[1] << " " << position[2] << " ";
        cout << "Intensity: " << intensity[0] << " " << intensity[1] << " " << intensity[2] << endl;

    }else{
        // error handling
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