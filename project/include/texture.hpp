//
// Created by Philipp Rauser on 29.09.24.
//

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <json/json.h>
#include <opencv2/opencv.hpp>
#include <mutex>
#include <condition_variable>
#include <string>

using std::string, cv::Vec3f;

class Texture{
protected:
   cv::Mat texture;
    int size;
    std::recursive_mutex mtx;     // Der Mutex um den Zugriff auf 'texture' zu schützen
    std::condition_variable_any cv; // Die Bedingungsvariable um auf 'texture' zu warten
    bool ready = false;
   int width;
   int height;
 public:
    bool isNormalTexture = true;
   Texture(Json::Value& input);
   Texture(const string& filename, int size);
    void setReady();
    bool isReady();
    Vec3f getColor(float x, float y);
    int getSize();
 };


#endif //TEXTURE_HPP
