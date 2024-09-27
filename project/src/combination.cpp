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
    intersectionInfo hit;
    hit.didHit = false;
    for(auto object : objects){
        object->intersection(ray, scene, &hit);
        if(hit.didHit){
            if(!closestHit->didHit || hit.t < closestHit->t){
                *closestHit = hit;
            }
        }
    }
}
