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

void Intersection::intersection(const Ray& ray, World* scene, intersectionInfo* closestHit){
    struct intersectionInfo intersectionHit;
    intersectionHit = {.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr};

    // it only saves the intersection info in closestHit if it hits all objects of the intersection
    for(auto object : objects){
        object->intersection(ray, scene, &intersectionHit);
        if(intersectionHit.didHit){
            if(objects.size() == 1){
                *closestHit = intersectionHit;
                break;
            }
            for(auto other : objects) {
                if(other == object ) continue;
                if(!other->isIncluded(intersectionHit.position)){
                    intersectionHit = {.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr};
                    break;
                }else {
                    *closestHit = intersectionHit;
                }
            }
        }
    }
}
