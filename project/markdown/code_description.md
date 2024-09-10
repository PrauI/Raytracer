# How the Code Works

---

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