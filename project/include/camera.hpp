#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <opencv2/opencv.hpp>
#include <string>
#include <json/json.h>

using cv::Vec4f, cv::Vec3f, cv::Mat, std::string;

/**
 * @brief Camera class handles the camera arguments as well as the image and outputing it
 * handles Parameters as Observer position, screen position, screen dimensions and dpi
 */
class Camera
{
private:
    /**
     * @brief holds Observer Position
     */
    Vec4f observerPosition;
    /**
     * @brief holds screen position
     */
    Vec4f screenPosition;
    /**
     * @brief holds screen dimensions
     */
    Vec4f screen;
    /**
     * @brief holds dpi / resolution of screen
     */
    int dpi;

public:
     /**
     * @brief holds image matrix / Pixel Values
     */
    Mat matrix;

    /**
     *@brief Default constructor for camera
     * setting camera to default values Observer position (0,0,-10), screen poistion (-1, -2, -5), screen dimensions (2, 4) and dpi 62
     */
     Camera();

    /**
     * @brief Construct a new Camera object
     *
     * @param observerPosition: position of the observer
     * @param P: position of the screen (lower right corner)
     * @param screen: dimensions of the screen
     * @param dpi: density per pixel of the screen
     */
    Camera(const Vec4f& observerPosition, const Vec4f& P, const Vec4f& screen, const int dpi);

    /**
     * @brief Construct a new Camera object
     *
     * @param Jason value which holds the camera parameters in the correct format
     */
    Camera(Json::Value& screen);


    /**
     * @brief Copy assignment operator
     */
     Camera& operator=(const Camera& other);

    /**
    * @brief Move assignment operator
    */
    Camera& operator=(Camera&& other);

     /**
     * @brief Capture the image and save it to a file
     *
     * @param filename: name of file to save to
     */
    void capture(const string& filename);
    Vec4f getPosition();
    Vec4f getObserver();
    int getDpi();
};

#endif