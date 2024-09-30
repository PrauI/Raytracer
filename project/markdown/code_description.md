# How the Code Works

---
## UML Diagram of all the classes

<img src="diagram.png">

## Reading the Input file and setting everything up

Every entity of our scene is organised in the World class. The World class bundels things like: Objects, Lights, The Camera and how they interact. It is the entry point in the Program as it reads the input file and distributes differen Json Objects to other parts of the program for further setup.

```cpp
void World::readFile(const string& filename) {
     // reading file and creating a Json::Value scene ...

    // setup camera
    Json::Value screen;
    screen = scene["screen"];
    camera = Camera(screen);

    // setup lights
    Json::Value medium;
    medium = scene["medium"];
    lightList.push_back(new Ambient(medium));

    // ...
}
```
The constructors for the different Entities in the scene such as Cameras or lights, get set up as their constructor takes the corresponding ```Json::Value``` as argument.

---
## Error Handling

While reading the Json file from the User it can happen, that things are missing or not in the correct format. To handle such exceptions we wrapped everything in try-catch Blocks, to provide helpfull Errormessages and providing default values if possible.

```cpp
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

    // ...
}
```

## Reading Objects

In our scene, physical entities such as spheres and half-spaces can undergo various transformations, including translation, scaling, and rotation. These transformations can be nested, leading to a complex hierarchy. To manage this complexity, we adopt a recursive approach to read and set up the objects within the scene.

Initial Setup: We begin by iterating over the elements in the Objects array. For each element, we invoke the setup function with the identity matrix as the initial transformation matrix, ensuring a clean slate for subsequent transformations.
Recursive Object Setup: The core of this process is the recursive method ```void World::setupObjects(Json::Value& object, Mat& matrix)```. When this function encounters a terminal object, such as a sphere or half-space, it constructs the object using the current transformation matrix and the provided Json::Value.
If the object parameter does not represent a terminal object but rather a transformation (e.g., translation, rotation, scaling), the current transformation matrix is updated accordingly. The function then recursively calls itself, passing the updated matrix and the next layer of the Json::Value structure. This recursive traversal continues until all nested transformations have been applied and the final object is fully configured

```cpp
void World::setupObjects(Json::Value& object, Mat& matrix){
    Object* finalObject = nullptr;
    if(object.isMember("sphere")) {
        finalObject = new Sphere(object["sphere"], matrix);
        objectList.push_back(finalObject);
    }
    // ... 
    else if(object.isMember("scaling")){
        // ...
        // simplified:
        // ...
        Mat translationMatrix = ...
        matrix = translationMatrix * matrix;
        setupObject(subject, matrix);
    }
    // ...
}
```

## Multithreading

To speed up the rendering process, we implemented multithreading. The rendering process is divided into multiple threads, each responsible for rendering a portion of the image. This approach allows us to leverage the full processing power of modern CPUs and significantly reduce the time required to generate the final image. 
By default the number of threads is set to 4 but can be changed by adding ```--threads <number> ``` to the command line arguments.
First the Matrix gets processed and split up according to the number of threads.

```cpp
    for(int i = 0; i < numThreads; ++i) {
            int startRow = i * blockSize;
            int endRow = (i == numThreads - 1) ? rows : (i + 1) * blockSize;
            threads.emplace_back([this, startRow, endRow, cols]() {
                this->calcMatrix(startRow, endRow, 0, cols);
            });
        }
        
        for(auto& thread : threads) {
        thread.join();
    }
```

Then each thread calculates the color of the pixels in its assigned block.

```cpp
for(int y = startRow; y < endRow; ++y){
        Vec3b* row_ptr = camera.matrix.ptr<Vec3b>(y);  // Zeiger auf die Zeile
    for(int x = startCol; x < endCol; ++x){
        closestHit = {.didHit = false, .t = INFINITY, .position = Vec4f(0.0), .normal = Vec4f(0.0), .dir = Vec4f(0.0), .object = nullptr};
        Vec4f delta {float(y) / dpi, float(x) / dpi,0,0};
        Vec4f S = cameraPosition + delta;
        Vec4f d = S - cameraObserver;
        cv::normalize(d,d);
        Ray ray = Ray{.dir = std::move(d), .position = std::move(S)};

        hit(ray, nullptr, &closestHit); // nullptr weil wir die kamera sind

        Vec3f color = {1.0,0.7,0.5};
        if(closestHit.didHit) color = mixLight(&closestHit, 0);

        else color = skyColor(ray.dir);
        row_ptr[x] = map255(color);
    }
}
```
This wierd architecture with pointers and extracting the row, saves us a tiny amount of computation time due to less cache misses.
```hit()``` calls an intersection test for every object in the scene and saves the closest hit in ```closestHit```.
```mixLight()``` then calculates the color of the pixel based on properties of the hit. When no object was hit,
```skyColor()``` calculates the color of the sky based on the direction of the ray.