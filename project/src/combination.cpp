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
/**
void Intersection::intersection(const Ray& ray, World* scene, intersectionInfo* closestHit){
    struct intersectionInfo intersectionHit;

    // it only saves the intersection info in closestHit if it hits all objects of the intersection
    for(auto object : objects){
        intersectionHit = {.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr};
        object->intersection(ray, scene, &intersectionHit);

        if(!intersectionHit.didHit) continue;

        if(objects.size() == 1){
            *closestHit = intersectionHit;
            break;
        }

        for(auto other : objects) {

            if(other == object ) continue;

            if(other->isIncluded(intersectionHit.position)){
                if(!closestHit->didHit || intersectionHit.t < closestHit->t) {
                    *closestHit = intersectionHit;
                }
            }else {
                intersectionHit = {.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr};
                *closestHit = intersectionHit;
                break;
            }
        }
    }

} */

void Intersection::intersection(const Ray& ray, World* scene, intersectionInfo* closestHit){
    std::vector<intersectionInfo> intersectionHits;
    intersectionHits.reserve(objects.size());

    // calculate all intersections
    for(int i = 0; i < objects.size(); i++){
        intersectionHits.push_back({.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr});
        objects[i]->intersection(ray, scene, &intersectionHits[i]);
    }

    for(auto intersectionHit : intersectionHits){
        if(!intersectionHit.didHit) continue;

        bool isInAll = true;
        for(auto object : objects) {
            if(object == intersectionHit.object) continue;
            if(!object->isIncluded(intersectionHit.position)){
                isInAll = false;
                break;
            }
        }

        if(isInAll) {
            if(!closestHit->didHit || intersectionHit.t < closestHit->t) {
                *closestHit = intersectionHit;
            }
        }
    }
}
