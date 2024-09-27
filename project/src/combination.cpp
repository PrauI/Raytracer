//
// Created by Philipp Rauser on 26.09.24.
//

#include "combination.hpp"
#include "object.hpp"
#include <iostream>

void Combination::addObject(Object* object){
    objects.push_back(object);
    std::cout << "Object added to Combination" << std::endl;
}

void Union::intersection(const Ray& ray, World* scene, intersectionInfo* closestHit){
    struct intersectionInfo unionHit;
     unionHit = {.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr};

    for(auto object : objects){
        object->intersection(ray, scene, &unionHit);
        if(unionHit.didHit){
            if(!closestHit->didHit || unionHit.t < closestHit->t){
                *closestHit = unionHit;
            }
        }
    }
}
