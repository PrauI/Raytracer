cmake_minimum_required(VERSION 3.10)

# Projektname und Sprache
project(Raytracer LANGUAGES CXX)

# Setze den Standard C++-Standard (z.B. C++17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include-Verzeichnisse hinzufügen
include_directories(include)

# Füge die Quell-Dateien hinzu
file(GLOB SOURCES "src/*.cpp" "main.cpp")

# Erstelle die ausführbare Datei
add_executable(Raytracer ${SOURCES})

# Json cpp
include_directories("/opt/homebrew/Cellar/jsoncpp/1.9.5/include/")
target_link_libraries(Raytracer PRIVATE "/opt/homebrew/Cellar/jsoncpp/1.9.5/lib/libjsoncpp.dylib")
# OpenCV
find_package(OpenCV REQUIRED)  # Sucht nach OpenCV
target_link_libraries(Raytracer PRIVATE ${OpenCV_LIBS})  # Fügt OpenCV hinzu