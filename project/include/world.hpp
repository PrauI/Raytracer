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
    std::vector<Light*> lightList;   /**< List of lights in the world. */
    std::vector<Object*> objectList; /**< List of objects in the world. */
public:
    Camera camera; /**< Camera object in the world. */
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
     * @brief shoots a ray for every pixel of the image and calculates the color
     * @param startRow
     * @param endRow
     * @param startCol
     * @param endCol
     */
    void calcMatrix(int startRow, int endRow, int startCol, int endCol);
    /**
     * @brief processes the matrix with a given number of threads
     * @param numThreads
     */
    void processMatrix(int numThreads);

    /**
     * @brief calculates objects to test collision with ray
     * @param ray light ray to check for collision
     * @param startingObject object to ignore in the collision check
     * @param closestHit intersection Info to store the closest hit
     */
    void hit(struct Ray& ray, Object* startingObject, intersectionInfo* closestHit);

 /**
     * @brief calculates light color at specific intersection points. Calls reflection and refraction
     * @param info intersection information
     * @param currentBounce current bounce of the ray
     * @param maxBounce maximum number of bounces
     * @return calculated light value as a Vec3f between 0 and 1
     */
    Vec3f mixLight(struct intersectionInfo* info, int currentBounce, int maxBounce);
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

/**
 * @brief returns the color of the sky based on angle of light vector
 * @param d direction of the ray
 * @return color of the sky
 */
Vec3f skyColor(const Vec4f& d);

Vec4f refractedDir(const float n, const float nt, const Vec4f& dir, const Vec4f& normal);

#endif // WORLD_HPP
