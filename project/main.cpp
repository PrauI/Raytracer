
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include "camera.hpp"
#include "world.hpp"

using cv::Vec4f, cv::Vec3f;

int main() {

    World world;

    // Speichern des Bildes als PNG-Datei
    std::string filename = "black_image.png";

    // camera.capture(filename);
    std::string inputFile = "scene.json";

    world.readFile(inputFile);
    world.calcMatrix();
    world.createImage(filename);
    return 0;
}