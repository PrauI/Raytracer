# Project Evaluation for G6 / PixelPionners

**Project grade :** 17 / 20 

**Bonus points :** 3.5 / 4 

**General comments :**

Good project in general, sad that I couldn't reproduce the images on my own computer ... 

---

## Program Usability (6.5 / 7 points)

### Description and easiness of the installation procedure : 2 / 2 points

RAS

### Code compilation without error (and warnings) : 2 / 2 points

RAS

### Usage documentation with application examples of use, tutorials, ... : 2 / 2 points

OK

### Execution of given application examples without error : 0.5 / 1 points

It works, but color from the input files appear to be ignore. For instance :

```bash
$ ./build/Raytracer output.png markdown/assets/jsons/niceOne.json 
dpi: 300
Dimensions: 2x4
Position: (-1, -2, -10)
Observer: (0, 0, -14)
Light Sources: 
No refracted color provided
Proceeding with default colors
No refracted color provided
Proceeding with default colors
No refracted color provided
Proceeding with default colors
No refracted color provided
Proceeding with default colors
No refracted color provided
Proceeding with default colors
No refracted color provided
Proceeding with default colors
Reading file: 0.00105385 seconds
Threads used: 6
Calculate Matrix: 63.9217 seconds
Das Bild wurde erfolgreich als output.png gespeichert.
Writing to file: 0.0132361 seconds
Computation time: 63.9361 seconds
```

produced this [output](./output.png).

---

## Code Documentation (4 / 6 points)

### Final UML class diagram : 2 / 2 points

RAS

### Docstring for functions and classes : 2 / 2 points

OK

### User-friendly generation of documentation (_e.g_ how to build documentation with Doxygen) : 0 / 2 points

None

---

## Development Quality (6.5 / 7 points)

### Capacity to use external libraries (_e.g_ OpenCV, Eigen, or else ...) : 2 / 2 points

...

### Implementation of required features : 2 / 2 points

RAS

### Automated testing procedures (to be used for further code development) : 0.5 / 1 points

At least it's there, but did not pass on my system. Are just execution tests tho ...

### Additional features (other primitives, python wrapping, etc ...) : 2 / 2 points

Textures, multithreading ok
