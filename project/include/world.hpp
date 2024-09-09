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
    std::vector<Light*> lightList;
    std::vector<Object*> objectList;
public:
    Camera camera;
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
    void calcMatrix(int startRow, int endRow, int startCol, int endCol);
    void processMatrix(int numThreads);
    void hit(struct Ray& ray, Object* startingObject, intersectionInfo* closestHit);
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
Vec3f skyColor(Vec4f& d);

#endif // WORLD_HPP
