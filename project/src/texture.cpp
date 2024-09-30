//
// Created by Philipp Rauser on 29.09.24.
//

#include "texture.hpp"
#include <mutex>
#include "world.hpp"
using std::string, cv::Vec3f, cv::Vec3b, std::cout, std::endl;

Texture::Texture(const string& filename, int size): size(size){
  cout << filename << endl;
    try{
      texture = cv::imread(filename, cv::IMREAD_COLOR);
      if(texture.empty()) throw std::runtime_error("Could not load Texture");
      cout << "Texture loaded: " << filename << endl;
    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        texture = cv::Mat(10, 10, CV_8UC3, cv::Scalar(255, 255, 255));
        size = 1;
        cout << "Default texture loaded" << endl;
        if(texture.empty()) std::cout << "shit" << std::endl;
    }
    width = texture.cols;
    height = texture.rows;
    setReady();
}

Texture::Texture(Json::Value& input): Texture(input["filename"].asString(), input["size"].asInt()){
    try{
        if(!input.isMember("filename")) throw std::runtime_error("No filename provided for Texture");
        if(!input.isMember("size")) throw std::runtime_error("No size provided for Texture");
    }catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        texture = cv::Mat(10, 10, CV_8UC3, cv::Scalar(255, 255, 255));
        size = 1;
        width = 10;
        height = 10;
    }
    setReady();

}

Vec3f Texture::getColor(float x, float y){
    std::lock_guard<std::recursive_mutex> lock(mtx);
    cv.wait(mtx, [this] {return ready;}); // wait until the texture is ready
    if(texture.empty()) throw std::runtime_error("Texture is empty");
    if (x < 0 || x > 1 || y < 0 || y > 1) throw std::out_of_range("x oder y außerhalb des gültigen Bereichs [0, 1]");

    int x_ = x * width;
    int y_ = y * height;
    if(x_ >= width) x_ = width - 1;
    if(y_ >= height) y_ = height - 1;
    cv::Vec3b color = texture.at<cv::Vec3b>(y_, x_);
    return map01(color);
 }

 void Texture::setReady(){
     std::lock_guard<std::recursive_mutex> lock(mtx);
     ready = true;
     cv.notify_all();
 }

 bool Texture::isReady(){
     std::lock_guard<std::recursive_mutex> lock(mtx);
     return ready;
 }

 int Texture::getSize(){
     return size;
 }