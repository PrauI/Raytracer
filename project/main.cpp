
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <chrono>
#include <thread>

#include "camera.hpp"
#include "world.hpp"

#define NUM_THREADS std::thread::hardware_concurrency()

using cv::Vec4f, cv::Vec3f;

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    World world;

    // Festlegen des Dateinamens fpr das zu speichernde Bild
    std::string filename = "black_image.png";

    // Festlegen des Dateinamens für die Eingabedatei
    std::string inputFile = "scene.json";

    // Einlesen der Parameter zum Erstellen einer Szene / Welt
    auto startReading = std::chrono::high_resolution_clock::now();
    world.readFile(inputFile);
    auto endReading = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> readingtime = endReading - startReading;
    std::cout << "Reading file: " << readingtime.count() << " seconds" << std::endl;
     // calcMatrix berchnet das Bild und speichert die Daten in einer Matrix
    auto startCalc = std::chrono::high_resolution_clock::now();
    // start Multithreading
    std::vector<std::thread> threads;

    int blockSizeY = (world.camera.matrix.rows + 1) / NUM_THREADS;

    for(int j = 0; j < NUM_THREADS; j++){
        int startY = j * blockSizeY;
        int endY = (j + 1) * blockSizeY;

        threads.emplace_back([=, &world]() { world.calcMatrix(startY, endY); });
    }
    
    for(auto& thread : threads){
        thread.join();
    }

    auto endCalc = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> calculationTime = endCalc - startCalc;
    std::cout << "Calculate Matrix: " << calculationTime.count() << " seconds" << std::endl;
    // createImage wandelt die zuvor berechnete Matrix in eine png-Datei um
    auto startWriting = std::chrono::high_resolution_clock::now();
    world.createImage(filename);
    auto endWriting = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> writingTime = endWriting - startWriting;
    std::cout << "Writing to file: " << writingTime.count() << " seconds" << std::endl;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Computation time: " << elapsed.count() << " seconds" << std::endl;
    return 0;
}