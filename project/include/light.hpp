#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <opencv2/opencv.hpp>
#include <json/json.h>
#include "object.hpp"

using cv::Vec3f, cv::Vec4f;

/**
 * @brief Base class for different types of lights in the scene.
 */
class Object;
class Light{
private:

public:
    /**
     * @brief Indicates if the light is an ambient light
     */
    bool isAmbient;
    /**
     * @brief Intensity of the light
     */
    Vec3f intensity;

    /**
     * @brief Default Constructor for Light
     */
     Light(): isAmbient(true), intensity(Vec3f(0.5, 0.5, 0.5)){};

    /**
     * @brief Pure virtual function to calculate the light value at a given intersection point.
     * @param info Intersection information.
     * @return Calculated light value as a Vec3f between 0 and 1.
     */
     virtual Vec3f lightValue(struct intersectionInfo* info) = 0;
};

/**
 * @brief Ambient light class
 */
class Ambient: public Light{
private:

public:
    /**
     * @brief Constructs an Ambient light object from JSON input.
     *
     * @param input JSON value containing the ambient light parameters.
     */
    Ambient(Json::Value& input);

    /**
     * brief Default Constructor for Ambient Light
     */
    Ambient();

    /**
     * @brief Calculates the light value at a given intersection point for ambient light.
     *
     * @param info Intersection information.
     * @return Calculated light value as a Vec3f between 0 and 1.
     */
    Vec3f lightValue(struct intersectionInfo* info);

};


/**
 * @brief Class representing a light source in the scene.
 */
class Source: public Light{
private:
     /**
     * @brief Position of the light source in scene.
     */
    Vec4f position;
public:
     /**
     * @brief Constructs a Source light object from JSON input.
     *
     * @param input JSON value containing the source light parameters.
     */
    Source(Json::Value& input);
    /**
     * @brief Calculates the light value at a given intersection point for the source light.
     *
     * @param info Intersection information.
     * @return Calculated light value as a Vec3f between 0 and 1.
     */
    Vec3f lightValue(struct intersectionInfo* info);
    Vec4f getPosition();
};

#endif