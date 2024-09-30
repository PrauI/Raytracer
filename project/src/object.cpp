#include "object.hpp"
#include "world.hpp"
#include "ray.hpp"
#include <json/json.h>
#include <iostream>
#include <cmath>
#include "texture.hpp"
#include <memory>

using std::cout, std::endl, cv::Mat;


void Object::setPosition(Json::Value& input){
    if(input.isArray() && input.size() == 3){
        position[0] = input[0].asFloat();
        position[1] = input[1].asFloat();
        position[2] = input[2].asFloat();
        position[3] = 1;
    }else{
            // error weil das muss halt so sein. sonst halt falsch dicka
    }
}

void Object::setDefaultPosition(){
    position[0] = 0; position[1] = 0; position[2] = 0; position[3] = 1;
}



Sphere::Sphere(Json::Value& input, Mat& matrix){

    // set position
    try
    {
        if(!input.isMember("position")) throw std::runtime_error("No Position provided for Sphere");
        Json::Value inputPosition = input["position"];
        if(!inputPosition.isArray() || inputPosition.size() != 3) throw std::runtime_error("No correct Format given for Sphere Position");
        setPosition(inputPosition);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "Proceeding with Position 0,0,0" << endl;
        setDefaultPosition();
    }
    
    // set radius
    try
    {
       if(!input.isMember("radius")) throw std::runtime_error("No Radius provided for Sphere");
       radius = input["radius"].asFloat();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "Proceeding with radius 1.0" << endl;
        radius = 1.0;
    }
    
    // set color
    try{
        if(!input.isMember("color")) throw std::runtime_error("No Color provided for Sphere");
        Json::Value inputColor = input["color"];
        setColor(inputColor);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with default colors" << endl;
        setDefaultColor();
    }

    // set Index
    try{
        if(!input.isMember("index")) throw std::runtime_error("No Index provided for Sphere");
        Json::Value Jindex = input["index"];
        setIndex(Jindex);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with Index: 1" << endl;
    }

    transformationMatrix = matrix.inv();
    normalizeTransformationMatrix(transformationMatrix);
}
Vec4f Object::getPosition(){ return position;}
Vec3f Object::getAmbient(){ return ambient;}
Vec3f Object::getDiffuse(){ return diffuse;}
float Object::getShininess() {return shininess;}
Vec3f Object::getSpecular(){ return specular;}
Vec3f Object::getReflected(){ return reflected; }
Vec3f Object::getRefracted(){ return refracted; }
float Object::getIndex(){ return index; }

void Object::setDefaultColor() {
    ambient = Vec3f(1.0, 1.0, 1.0);
    diffuse = Vec3f(1.0, 1.0, 1.0);
    specular = Vec3f(1.0, 1.0, 1.0);
    reflected = Vec3f(1.0, 1.0, 1.0);
    refracted = Vec3f(1.0, 1.0, 1.0);
    shininess = 0.0;
}

void Object::setColor(Json::Value& color){
    // todo try catch

    for(int i = 0; i < 3; i++){
        ambient[i] = color["ambient"][i].asFloat();
        diffuse[i] = color["diffuse"][i].asFloat();
        specular[i] = color["specular"][i].asFloat();
        reflected[i] = color["reflected"][i].asFloat();
        refracted[i] = color["refracted"][i].asFloat();
    }
    shininess = color["shininess"].asFloat();

    if(color.isMember("texture")) {
        texture = std::make_shared<Texture>(color["texture"]);
    }
}

void Object::setIndex(Json::Value& Jindex){
    try{
        index = Jindex.asFloat();
    }catch(const std::exception& e){
        cout << e.what() << endl;
        index = 1.0;
        cout << "Proceeding with Index: 1" << endl;
    }
}

void Sphere::intersection(const struct Ray& ray, World* scene, intersectionInfo* closesHit){
    Mat resultD = transformationMatrix * ray.dir;
    Vec4f dm = resultD;

    Vec4f C = getPosition();

    Mat resultS = transformationMatrix * ray.position;
    Vec4f Sm = resultS;

    Vec4f L = C - Sm;
    float tca = L.dot(dm);
    if(tca < 0) return; // sphere is behind camera

    float d2 = L.dot(L) - tca * tca;
    if(d2 > radius * radius) return; // ray misses sphere

    float thc = sqrt(radius * radius - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;

    if(t0 > t1) std::swap(t0,t1);
    if(t0 < 0) {
        t0 = t1;
        if(t0 < 0) return;
    }

    if(t0 >= closesHit->t) return;
    closesHit->t = t0;
    closesHit->didHit = true;
    closesHit->object = this;
    closesHit->dir = -dm;

    closesHit->position = Sm + dm * t0;
    closesHit->normal = (closesHit->position - C) / radius;

    // change normal based on texture
    if(texture != nullptr) {
        Vec3f normalMap = getTextureColor(closesHit->position);
        cv::normalize(normalMap, normalMap);
        Vec4f normalMap4f(normalMap[0], normalMap[1], normalMap[2], 0.0f);
        for(int i = 0; i < 3; i++) {
            normalMap4f[i] = normalMap4f[i] * 2 - 1;
        }
        closesHit->normal = closesHit->normal + normalMap4f *0.3;

    }

    // cout << "intersection: " << value << endl;
    return;

}

bool Sphere::isIncluded(const Vec4f &point) {
    return pow(point[0] - position[0], 2) + pow(point[1] - position[1], 2) + pow(point[2] - position[2], 2) <= pow(radius, 2) + 0.0001;
}

float length(Vec4f& x){
    return sqrt(pow(x[0],2) + pow(x[1], 2) + pow(x[2], 2) + pow(x[3], 2));
}


bool Halfspace::isIncluded(const Vec4f &point) {
    Vec4f dirToPos = point - getPosition();
    return dirToPos.dot(normal) == 0;
}


void Halfspace::setNormal(Json::Value& input){
    // we already checked if input is in the right format beore calling the function
    for(int i = 0; i < 3; i++){
        normal[i] = input[i].asFloat();
    }
    normal[3] = 0;
    cv::normalize(normal, normal);
}

void Halfspace::setDefaultNormal(){
    normal = Vec4f(0.0);
}

Halfspace::Halfspace(Json::Value& input, Mat& matrix){
    // set position
    try{
        if(!input.isMember("position")) throw std::runtime_error("No Position provided for Halfspace");
        Json::Value inputPosition = input["position"];
        if(!inputPosition.isArray() || inputPosition.size() != 3) throw std::runtime_error("No correct Format given for Halfspace Positon");
        setPosition(inputPosition);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with position 0,0,0" << endl;
        setDefaultPosition();
    }
    // set normal
    try{
        if(!input.isMember("normal")) throw std::runtime_error("No Normal provided for Halfspace");
        Json::Value inputNormal = input["normal"];
        if(!inputNormal.isArray() || inputNormal.size() != 3) throw std::runtime_error("No correct Format given for Halfspace Normal");
        setNormal(inputNormal);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with normal 1,0,0" << endl;
        setDefaultNormal();
    }

    // set color
    try{
        if(!input.isMember("color")) throw std::runtime_error("No Color provided for Halfspace");
        Json::Value inputColor = input["color"];
        setColor(inputColor);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with default colors" << endl;
        setDefaultColor();
    }

     // set Index
    try{
        if(!input.isMember("index")) throw std::runtime_error("No Index provided for Halfspace");
        Json::Value Jindex = input["index"];
        setIndex(Jindex);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with Index: 1" << endl;
        index = 1;
    }

    transformationMatrix = matrix.inv();
    normalizeTransformationMatrix(transformationMatrix);
}

void Halfspace::intersection(const struct Ray& ray, World* scene, intersectionInfo* closesHit){
    Mat resultD = transformationMatrix * ray.dir;
    Vec4f dm = resultD;
    Mat resultS = transformationMatrix * ray.position;
    Vec4f Sm = resultS;
 
   // since a halfspace is an infinite plane the there is either an intersection point or the ray coming from the camera is parallel to the plane
   float scalarValue = this->normal.dot(dm);
   // cout << "normal: " << normal << " d: " << dm << " scalar: " << scalarValue << endl;
   if(scalarValue > -0.0001) return; // they are parallel and there is no intersection
   Vec4f pos = getPosition();

   float t = (pos - Sm).dot(normal) / scalarValue;
    if(t < 0) return; // intersection Point behind camera

    if(t >= closesHit->t) return;


    closesHit->t = t;
    closesHit->dir = -dm;
    closesHit->didHit = true;

    Vec4f intersectionPoint = Sm + t * dm;
    closesHit->position = intersectionPoint;
    closesHit->object = this;
    closesHit->normal = normal;

    // change normal based on normalmap
    if(texture == nullptr) return;
    Vec3f normalMap = getTextureColor(closesHit->position);
    cv::normalize(normalMap, normalMap);
    Vec4f normalMap4f(normalMap[0], normalMap[1], normalMap[2], 0.0f);
    for(int i = 0; i < 3; i++) {
        normalMap4f[i] = normalMap4f[i] * 2 - 1;
    }
    closesHit->normal = closesHit->normal + normalMap4f *0.5;

    return;
}

void normalizeTransformationMatrix(cv::Mat& transformationMatrix) {
    // Beispiel - Richtungsvektor
    Vec4f exampleDir = Vec4f(1,0,0,0);
    Mat transformationMat = transformationMatrix * exampleDir;
    Vec4f transformationDir = transformationMat;

    // calculate length of transformed Vector
    float length = std::sqrt(transformationDir.dot(transformationDir));
    transformationMatrix /= length;
}

Vec4f Sphere::getNormal(const Vec4f &point) {
    const float epsilon = 0.0001;
    float squaredDistance = pow(point[0] - position[0], 2) + pow(point[1] - position[1], 2) + pow(point[2] - position[2], 2);
    if(squaredDistance >= pow(radius, 2) - epsilon && squaredDistance <= pow(radius, 2) + epsilon) {
        // the point exists on the sphere surface
        Vec4f n = (point - position) / radius;
        if(texture != nullptr) {
            Vec3f normalMap = getTextureColor(point);
            cv::normalize(normalMap, normalMap);
            Vec4f normalMap4f(normalMap[0], normalMap[1], normalMap[2], 0.0f);
            for(int i = 0; i < 3; i++) {
                normalMap4f[i] = normalMap4f[i] * 2 - 1;
            }
            n = n + normalMap4f *0.3;
            return n;

        }
    }
    return Vec4f(0.0);
}

Vec4f Halfspace::getNormal(const Vec4f &position) {
    // check if point is on plane
    if(normal.dot(position - getPosition()) == 0) return normal;
    return Vec4f(0.0);
}

CombinationWrapper::CombinationWrapper(Combination *combination) : combination(combination) {
        setDefaultPosition();
        setDefaultColor();
}

Vec4f CombinationWrapper::getNormal(const Vec4f &position) {

    return combination->getNormal(position);
}




void CombinationWrapper::intersection(const struct Ray& ray, World* scene, intersectionInfo* closesHit) {
    if(combination != nullptr) combination->intersection(ray, scene, closesHit);
    else {
        std::cout << "No combination set" << std::endl;
        closesHit->didHit = false;
    }
}

bool CombinationWrapper::isIncluded(const Vec4f &point) {
    return combination->isIncluded(point);
}


bool Object::hasTexture() {
    if(texture == nullptr) return false;
    return true;
}

Vec3f Sphere::getTextureColor(const Vec4f &point) {
    if(texture == nullptr) return Vec3f(0.0);
    // calculate lat and long
    Vec4f point_ = point - getPosition();
    float phi = atan2(point_[1], point_[0]);
    float theta = acos(point_[2] / radius);
    float longitude = (phi + M_PI) / (2 * M_PI);
    float latitude = 1 - (theta / M_PI);

    return texture->getColor(longitude, latitude);
}

Vec3f Halfspace::getTextureColor(const Vec4f &point) {
    if(texture == nullptr) return Vec3f(0.0);

    // calculate two orthogoanl vektors to the normal
    Vec4f v = Vec4f(1,0,0,0);
    // check if v is different then normal
    if(v.dot(normal) == 1) v = Vec4f(0,1,0,0);
    Vec4f t1 = crossProduct(normal, v);
    Vec4f t2 = crossProduct(normal, t1);
    cv::normalize(t1, t1);
    cv::normalize(t2, t2);

    Vec4f point_ = point - getPosition();

    // create the Matrix A from the vectors v1 and v2 (4x2 Matrix)
    cv::Mat A = (cv::Mat_<float>(4,2) << t1[0], t2[0], t1[1], t2[1], t1[2], t2[2], t1[3], t2[3]);
    // create the matrx for point p (4x1 Matrix)
    cv::Mat P = (cv::Mat_<float>(4,1) << point_[0], point_[1], point_[2], point_[3]);

    // solve the coefficients a and b using least squares
    cv::Mat result;
    cv::solve(A, P, result, cv::DECOMP_QR); // QR decomposition
    float a = result.at<float>(0,0);
    float b = result.at<float>(1,0);



    return texture->getColor(mapToUnitInterval(a, texture->getSize()), mapToUnitInterval(b, texture->getSize()));

}

Vec3f CombinationWrapper::getTextureColor(const Vec4f &point) {
    if(texture == nullptr) return Vec3f(0,0,0);
    return Vec3f(0,0,0);
}

Vec4f crossProduct(const cv::Vec4f& a, const cv::Vec4f& b) {
    // carefull ! this one ignores the fourth component!!
    return cv::Vec4f(
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0],
        0.0f
    );
}

float mapToUnitInterval(float x, float range) {
    float result = std::fmod(x, range);
    if (result < 0) {
        result += range;
    }
    return (result - range / 2.0f) / range + 0.5f;
}


Triangle::Triangle(Json::Value &input, Mat &matrix) {
    try {
        if(!input.isMember("position")) throw std::runtime_error("No Position provided for Triangle");
        Json::Value inputPosition = input["position"];
        if(!inputPosition.isArray() || inputPosition.size() != 3) throw std::runtime_error("No correct Format given for Triangle Position");
        for(int i = 0; i < 3; i++){
            if(!inputPosition[i].isArray() || inputPosition[i].size() != 3) throw std::runtime_error("No correct Format given for Triangle Position");
        }
        for(int i = 0; i < 3; i++) {
            v0[i] = inputPosition[0][i].asFloat();
            v1[i] = inputPosition[1][i].asFloat();
            v2[i] = inputPosition[2][i].asFloat();
        }
        v0[3] = 1; v1[3] = 1; v2[3] = 1;

    }catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        cout << "Proceeding with default triangle" << endl;
        v0 = Vec4f(0,0,0,1);
        v1 = Vec4f(1,0,0,1);
        v2 = Vec4f(0,1,0,1);
    }


    // set normal
    try{
        if(!input.isMember("normal")) throw std::runtime_error("No Normal provided for Halfspace");
        Json::Value inputNormal = input["normal"];
        if(!inputNormal.isArray() || inputNormal.size() != 3) throw std::runtime_error("No correct Format given for Halfspace Normal");
        for(int i = 0; i < 3; i++){
            normal[i] = inputNormal[i].asFloat();
        }
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with normal 1,0,0" << endl;
        normal = Vec4f(0,0,-1,0);
    }

    // set color
    try{
        if(!input.isMember("color")) throw std::runtime_error("No Color provided for Halfspace");
        Json::Value inputColor = input["color"];
        setColor(inputColor);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with default colors" << endl;
        setDefaultColor();
    }

    // set Index
    try{
        if(!input.isMember("index")) throw std::runtime_error("No Index provided for Halfspace");
        Json::Value Jindex = input["index"];
        setIndex(Jindex);
    }catch(const std::exception& e){
        std::cerr << e.what() << endl;
        cout << "Proceeding with Index: 1" << endl;
        index = 1;
    }

    u = v1 - v0;
    v = v2 - v0;
    uPerp = crossProduct(u, normal);
    vPerp = crossProduct(v, normal);

    transformationMatrix = matrix.inv();
    normalizeTransformationMatrix(transformationMatrix);
}


void Triangle::intersection(const struct Ray &ray, World *scene, intersectionInfo *closesHit) {
    cv::Mat resultD = transformationMatrix * ray.dir;
    Vec4f dm = resultD;
    cv::Mat resulS = transformationMatrix * ray.position;
    Vec4f Sm = resulS;

    Vec4f P = crossProduct(dm, v);
    float det = u.dot(P);

    if(det < 0.0001 && det > -0.0001) return;

    float invDet = 1 / det;

    Vec4f T = Sm - v0;
    float u_ = T.dot(P) * invDet;
    if(u_ < 0 || u_ > 1) return;

    Vec4f Q = crossProduct(T, u);
    float v_ = dm.dot(Q) * invDet;
    if(v_ < 0 || (u_ + v_) > 1) return;

    float t = v.dot(Q) * invDet;
    if(t < 0) return;

    if(!closesHit->didHit || t < closesHit->t) {
        closesHit->t = t;
        closesHit->didHit = true;
        closesHit->object = this;
        closesHit->dir = -dm;
        closesHit->position = Sm + t * dm;
        closesHit->normal = getNormal(closesHit->position);
    }

}

Vec4f Triangle::getNormal(const Vec4f &position) {
    return normal;
}

bool Triangle::isIncluded(const Vec4f& point) {
    // Transform the point using the inverse transformation matrix
    cv::Mat resultP = transformationMatrix * point;
    Vec4f P = resultP;

    // Compute vectors from the triangle's vertices to the point
    Vec4f v0P = P - v0;
    Vec4f v1P = P - v1;
    Vec4f v2P = P - v2;

    // Calculate dot products
    float dot00 = u.dot(u);
    float dot01 = u.dot(v);
    float dot02 = u.dot(v0P);
    float dot11 = v.dot(v);
    float dot12 = v.dot(v0P);

    // Compute barycentric coordinates
    float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
    float u_ = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v_ = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // Check if point is in triangle
    return (u_ >= 0) && (v_ >= 0) && (u_ + v_ <= 1);
}

Vec3f Triangle::getTextureColor(const Vec4f& point) {

    return Vec3f(0.0);
}