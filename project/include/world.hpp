#ifndef WORLD_HPP
#define WORLD_HPP

#include <json/json.h>
#include <opencv2/opencv.hpp>
#include <string>
#include "camera.hpp"
#include "light.hpp"
#include "object.hpp"
#include <vector>

using std::string, cv::Vec3f, cv::Vec4f, cv::Vec3b;

class Object;
class Light;
struct intersectionInfo;

/**
 * @brief Class representing the world / scene containing objects, lights, and a camera.
 */
class World {
    Camera camera;                      /**< Camera object for the world. */
    std::vector<Light*> lightList;      /**< List of lights in the world. */
    std::vector<Object*> objectList;    /**< List of objects in the world. */
public:
    World();

    /**
     * @brief Reads the world configuration from a file.
     * @param filename Name of the file containing the world configuration.
     */
    void readFile(const string& filename);

    /**
     * @brief Creates an image of the world and saves it to a file.
     * @param filename Name of the file to save the image.
     */
    void createImage(const string& filename);

   /**
    * @brief Reads objects from a JSON value and adds them to the world.
    *
    * Reads a list of objects from a JSON value. It iterates over each object in the JSON array,
    * initializes a transformation matrix to the identity matrix, and calls the setupObjects function to preocess each obejct and apply any transformations.
    *
    * @param objects JSON value containing the objects.
    */
    void readObjects(Json::Value& objects);

   /**
    * @brief Sets up objects in the world using a JSON value and a transformation matrix.
    *
    * Processes a single object from a JSON value and applies transformations such as scaling and translation.
    * It creates instances of specific object types (e.g. Sphere, Halfspace) and adds them to the objectList.
    * If the object contains nested transformations, it recursively processes them.
    *
    * @param object JSON value containing the object parameters.
    * @param matrix Transformation matrix.
    */
    void setupObjects(Json::Value& object, Mat& matrix);

     /**
     * @brief Calculates the matrix for the world.
     *
     * iterates over each pixel in the camera's image matrix, calculates the direction of a ray from the camera's positon
     * through each pixel, and determines the closest intersection of the ray with objects in the scene.
     * If an intersection is found, it calculates the color at the intersection point using the mixLight function.
     * The resulting color is stored in the image matrix, after mapping to a 0-255 range. If no intersection is found,
     * default color is used.
     */
    void calcMatrix();

     /**
    * @brief Mixes the light at an intersection point.
    *
    * This function calculates the resulting light color at a given intersection point by considering all the light sources in the scene.
    * It checks if the intersection point is in shadow by determining if any objects block the light from each light source.
    * If the point is not in shadow, it adds the light contribution from the light source to the final color.
    * The resulting color is clamped to the range [0, 1].
    *
    * @param info Pointer to the intersection information.
    * @return Mixed light value as a Vec3f.
    */
    Vec3f mixLight(struct intersectionInfo* info);
};

/**
 * @brief mappes a Vec3f color from 0-1 to a Vec3b color from 0-255
 * @param color
 * @return Vec3b color ranging from 0-255
 */
Vec3b map255(const Vec3f& color);

/**
 * @brief adds two light colors together
 * @param color1
 * @param color2
 * @return
 */
Vec3f addLight(Vec3f& color1, Vec3f& color2);

#endif // WORLD_HPP
