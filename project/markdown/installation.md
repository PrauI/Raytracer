
# Instalation guide

### Clone the git repository on your local machine
    
```bash
git clone https://collaborating.tuhh.de/e-10/teaching/oop/exo/2024/g6/pixelpionners.git
```
### Install the required libraries

Linux:
```bash
sudo apt-get install libopencv-dev
sudo apt-get install libjsoncpp-dev
```

MacOS:
```bash
brew install opencv
brew install jsoncpp
```

### On MacOS
when installing jsoncpp with homebrew it comes without cmake support. Therefor you have to add the include path and lib path manualy

```cmake
# Json cpp
include_directories("dir/to/jsoncpp/.../include/")
target_link_libraries(Raytracer PRIVATE "/dir/to/jsoncpp/.../lib/libjsoncpp.dylib")
```

### Compile the project

```bash
cd pixelpioneers/project
mkdir build
cd build
cmake ..
make
```
