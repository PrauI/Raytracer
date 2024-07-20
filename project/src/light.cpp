#include "light.hpp"
#include "object.hpp"
#include "world.hpp"
#include <iostream>
#include <math.h>

using std::cout;
using std::endl;

Ambient::Ambient(Json::Value& medium){
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

Source::Source(Json::Value& input){
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

Vec3f Ambient::lightValue(const Vec4f& V, const Vec4f& P, const Vec4f& N, Object* object){
    Vec3f Ea;
    Vec3f Ka = object->getAmbient();
    for(int i = 0; i < 3; i++){
        Ea[i] = intensity[i] * Ka[i];
    }
    return Ea;
}

Vec3f Source::lightValue(const Vec4f& V, const Vec4f& P, const Vec4f& N, Object* object){
    // Diffuse component
    Vec3f Ed;
    Vec3f Kd = object->getDiffuse();
    Vec4f L = position - P;
    cv::normalize(L,L);
    float scalar = scalarProduct(L, N);

    // specular component
    Vec3f Es;
    Vec3f Ks = object->getSpecular();
    Vec4f R = L - 2* scalarProduct(L, N) * N;
    cv::normalize(V,V);
    float scalar2 = scalarProduct(R, V);
 
     for(int i = 0; i < 3; i++){
        Ed[i] = Kd[i] * intensity[i] * pow(scalar, object->getShininess());
        Es[i] = Ks[i] * intensity[i] * pow(scalar2, object->getShininess());

    }


    return addLight(Es, Ed);

}