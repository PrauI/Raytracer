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

bool Union::isIncluded(const Vec4f &point) {

    for(auto object: objects) {
        if(object->isIncluded(point)) return true;
    }
    return false;
}


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

bool Intersection::isIncluded(const Vec4f &point) {
    for(auto object: objects) {
        if(!object->isIncluded(point)) return false;
    }
    return true;
}


void Exclusion::intersection(const Ray& ray, World* scene, intersectionInfo* closestHit){
    std::vector<intersectionInfo> intersectionHits;
    intersectionHits.reserve(objects.size());

    // calculate all intersections
    for(int i = 0; i < objects.size(); i++){
        intersectionHits.push_back({.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr});
        objects[i]->intersection(ray, scene, &intersectionHits[i]);
    }

    for(auto intersectionHit : intersectionHits){
        if(!intersectionHit.didHit) continue;

        bool isOnlyInOne = true;
        for(auto object : objects) {
            if(object == intersectionHit.object) continue;
            if(object->isIncluded(intersectionHit.position)){
                isOnlyInOne = false;
                break;
            }
        }

        if(isOnlyInOne) {
            if(!closestHit->didHit || intersectionHit.t < closestHit->t) {
                *closestHit = intersectionHit;
            }
        }
    }
}

bool Exclusion::isIncluded(const Vec4f &point) {
    int isIn = 0;
    for(auto object: objects) {
        if(object->isIncluded(point)) isIn++;
    }
    return isIn == 1;
}

Vec4f Combination::getNormal(const Vec4f &position) {
    if(objects.empty()) return Vec4f(0.0);
    for(auto object : objects){
        Vec4f normal = object->getNormal(position);
        if(normal != Vec4f(0.0)) return normal;
    }
    return Vec4f(0.0);
}