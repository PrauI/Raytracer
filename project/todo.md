# Raytracing project

- [x] Error handling in the Camera input 
- [x] Error handling in der world::readFile (see all the try catch blocks from the other inputs)
- [ ] error handling überall wo ich (Philipp) kommentar geschrieben hab, dass error handling fehlt
- [x] Reflected light
- [x] refracted light
- [x] translation
- [x] scaling
- [x] rotation
- [ ] in main.cpp calcMatrix und createImage zusammenfassen 
- [x] in code descriptions
- [x] alle todos in code abarbeiten
- [x] Philipp: sphere default color method / function
  - [x] add to sphere and halfspace constructor
- [x] unionize objects and add to world::setupObjects
- [ ] try catch in Object::setColor
- [x] todo in Halfspace::intersection
- [x] world:readObjects in try catch umwandeln
- [x] evtl todo: himmelsfarbe ändern, basierend auf höhe / winkel des strahls
- [x] der todo in world::mixlight
- [ ] document or code in a markdown file and do the readme stuff expected by the professor
    - [ ] usage description
    - [ ] ULM diagram
    - [x] installation guide
    - [ ] documentation of how the program works
- [x] get rid of the warnings
- [x] replace all scalarProduct() with the opencv dotproduct function
- [ ] automatic testing   
- [ ] optional: optimize code performance even more
- [ ] testing
  - [ ] more tests
  - [ ] solve exisitng test which doesnt work
- [x] rausfinden was mit index in ambient in der json gemeint ist
- [x] das recursion aus der json einfügen

## additional features
we have to implement at least two additional features. some ideas:

- [x] more shapes, like triangle
- [ ] sky Box / sky sphere
- [x] texture mapping ( i think its similaar to skysphere (at least for spheres))
- [ ] directional light

