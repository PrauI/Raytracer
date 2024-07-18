#include "light.hpp"
#include <iostream>

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