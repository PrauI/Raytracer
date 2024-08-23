
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <chrono>

#include "camera.hpp"
#include "world.hpp"

using cv::Vec4f, cv::Vec3f;

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    World world;

    // Festlegen des Dateinamens fpr das zu speichernde Bild
    std::string filename = "black_image.png";

    // Festlegen des Dateinamens für die Eingabedatei
    std::string inputFile = "scene.json";

    // Einlesen der Parameter zum Erstellen einer Szene / Welt
    world.readFile(inputFile);
     // calcMatrix berchnet das Bild und speichert die Daten in einer Matrix
    world.calcMatrix();
    // createImage wandelt die zuvor berechnete Matrix in eine png-Datei um
    world.createImage(filename);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Computation time: " << elapsed.count() << " seconds" << std::endl;
    return 0;
}