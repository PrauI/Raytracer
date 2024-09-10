#include "camera.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

using cv::Vec4f, cv::Vec3f, std::cout, std::endl, std::string;

Camera::Camera(const Vec4f& observerPosition, const Vec4f& P, const Vec4f& screen, const int dpi): observerPosition(observerPosition), screenPosition(P), screen(screen), dpi(dpi) {};
Camera::Camera(): Camera(Vec4f(0,0,-10,1), Vec4f(-1,-2,-5,1), Vec4f(2,4,0,0), 100) {};

Camera::Camera(Json::Value& jfile){

    // dpi
    try {
        if(jfile.isMember("dpi")) {
            dpi = jfile["dpi"].asInt();
            if(dpi <= 0) throw std::runtime_error("'dpi can't be smaller or equal to one");
            if(dpi > 300) throw std::runtime_error("'dpi' can#t be greater then 300");
        } else throw std::runtime_error("Kein 'dpi' Mitglied in der Datei gefunden");
    } catch(const std::exception& e) {
        dpi = 62;
        std::cerr << "Warnung: " << e.what() << ". Standardwert '62' wird verwendet." << std::endl;
    }
    cout << "dpi: " << dpi << endl;

    // dimensions
    try{
        if(jfile.isMember("dimensions")){
            const Json::Value dimensions = jfile["dimensions"];
            if (dimensions.isArray() && dimensions.size() == 2) {
                    screen[0] = dimensions[0].asInt();
                    screen[1] = dimensions[1].asInt();
                    screen[2] = 0; screen[3] = 0;
            }else throw std::runtime_error("No correct format given");
        }else throw std::runtime_error("No 'dimensions' member in file");
    }catch(const std::exception& e){
        std::cerr << "! Warning 'dimensions': " << e.what() << ". Using 2/4" << endl;
        screen[0] = 2;
        screen[1] = 4;
        screen[2] = 0; screen[3] = 0;

    }
    cout << "Dimensions: " << screen[0] << "x" << screen[1] << std::endl;

    // position
    try{
        if (jfile.isMember("position")) {
            const Json::Value& position = jfile["position"];
            if (position.isArray() && position.size() == 3) {
                screenPosition[0] = position[0].asInt();
                screenPosition[1] = position[1].asInt();
                screenPosition[2] = position[2].asInt();
                screenPosition[3] = 1;
            }else throw std::runtime_error("No correct format given.");
        } else throw std::runtime_error("No 'position' member in file.");
    }catch(const std::exception& e){
        std::cerr << "! Warning 'position': " << e.what() << " Using 0, 0, 0" << endl;
        screenPosition[0] = 0; screenPosition[1] = 0; screenPosition[2] = 0; screenPosition[3] = 1;
    }
    cout << "Position: (" << screenPosition[0] << ", " << screenPosition[1] << ", " << screenPosition[2] << ")" << std::endl;
   
    // observer
    try{
        if (jfile.isMember("observer")) {
            const Json::Value& observer = jfile["observer"];
            if (observer.isArray() && observer.size() == 3) {
                observerPosition[0] = observer[0].asInt();
                observerPosition[1] = observer[1].asInt();
                observerPosition[2] = observer[2].asInt();
                observerPosition[3] = 1;
            }else throw std::runtime_error("No correct format given.");
        }else throw std::runtime_error("No 'observer' member in file.");
    }catch(const std::exception& e){
       std::cerr << "! Warning 'observer': " << e.what() << " Using 1, 2, -10" << endl; 
       observerPosition[0] = 1; observerPosition[1] = 2; observerPosition[2] = -10; observerPosition[3] = 1;
    }
    cout << "Observer: (" << observerPosition[0] << ", " << observerPosition[1] << ", " << observerPosition[2] << ")" << std::endl;

    // initialise Matrix
    int height = dpi*screen[0];
    int width = dpi*screen[1];
    matrix = cv::Mat(height, width, CV_8UC3, cv::Scalar(255,255,255));
}

// copy assigment operator
Camera& Camera::operator=(const Camera& other){
    if(this == &other) return *this; // self-assignment check

    // copy Camera
    observerPosition = other.observerPosition;
    screenPosition = other.screenPosition;
    screen = other.screen;
    dpi = other.dpi;
    matrix = other.matrix;
    return *this;
}

// move assignment operator
Camera& Camera::operator=(Camera&& other) {
    if(this == &other) return *this;

    // Move Camera
    observerPosition = std::move(other.observerPosition);
    screenPosition = std::move(other.screenPosition);
    screen = std::move(other.screen);
    dpi = std::move(other.dpi);
    matrix = std::move(other.matrix);
    return *this;
}

void Camera::capture(const string& filename){

    if(cv::imwrite(filename, matrix)) {
        cout << "Das Bild wurde erfolgreich als " << filename << " gespeichert." << endl;
    }
    else{
        cout << "Fehler beim Speichern des Bildes." << endl;
    }
}

Vec4f Camera::getPosition(){
    return screenPosition;
}

Vec4f Camera::getObserver(){
    return observerPosition;
}

int Camera::getDpi(){
    return dpi;
}