//
// Created by Philipp Rauser on 26.09.24.
//

#ifndef COMBINATION_H
#define COMBINATION_H

#include "object.hpp"
#include "world.hpp"
#include "ray.hpp"
#include <vector>


class Object;
class World;
struct intersectionInfo;


class Combination {
protected:
    std::vector<Object*> objects;
public:
    void addObject(Object* object);
    virtual void intersection(const Ray& ray, World* scene, intersectionInfo* closesHit) = 0;


};

class Union : public Combination{
public:
    void intersection(const Ray& ray, World* scene, intersectionInfo* closesHit) override;
};



#endif //COMBINATION_H
