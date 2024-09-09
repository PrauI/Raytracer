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
     * @brief Construct a new Camera object
     *
     * @param observerPosition: position of the observer
     * @param P: position of the screen (lower right corner)
     * @param screen: dimensions of the screen
     * @param dpi: density per pixel of the screen
     */
    Camera(Vec4f& observerPosition, Vec4f& P, Vec4f& screen, int dpi);
    /**
     * @brief Construct a new Camera object
     *
     * @param Jason value which holds the camera parameters in the correct format
     */
    Camera(Json::Value& screen);
    Camera();
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