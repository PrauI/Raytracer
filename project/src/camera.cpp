#include "camera.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using cv::Vec4f, cv::Vec3f, std::cout, std::endl;

Camera::Camera(Vec4f& observerPosition, Vec4f& P, Vec4f& screen, int dpi): observerPosition(observerPosition), screenPosition(P), screen(screen), dpi(dpi) {};

Camera::Camera(){}

Camera::Camera(Json::Value& jfile){
    if(jfile.isMember("dpi")) {
        dpi = jfile["dpi"].asInt();
        cout << "dpi: " << dpi << endl;
    }

    if(jfile.isMember("dimensions")){
        const Json::Value dimensions = jfile["dimensions"];
        if (dimensions.isArray() && dimensions.size() == 2) {
                screen[0] = dimensions[0].asInt();
                screen[1] = dimensions[1].asInt();
                screen[2] = 0; screen[3] = 0;
                cout << "Dimensions: " << screen[0] << "x" << screen[1] << std::endl;
            }
    }

    if (jfile.isMember("position")) {
            const Json::Value& position = jfile["position"];
            if (position.isArray() && position.size() == 3) {
                screenPosition[0] = position[0].asInt();
                screenPosition[1] = position[1].asInt();
                screenPosition[2] = position[2].asInt();
                screenPosition[3] = 1;
                cout << "Position: (" << screenPosition[0] << ", " << screenPosition[1] << ", " << screenPosition[2] << ")" << std::endl;
            }
        }
    
    if (jfile.isMember("observer")) {
            const Json::Value& observer = jfile["observer"];
            if (observer.isArray() && observer.size() == 3) {
                observerPosition[0] = observer[0].asInt();
                observerPosition[1] = observer[1].asInt();
                observerPosition[2] = observer[2].asInt();
                observerPosition[3] = 1;
                cout << "Observer: (" << observerPosition[0] << ", " << observerPosition[1] << ", " << observerPosition[2] << ")" << std::endl;
            }
    }

    // initialise Matrix
    int height = dpi*screen[0];
    int width = dpi*screen[1];
    matrix = cv::Mat(height, width, CV_8UC3, cv::Scalar(255,255,255));
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