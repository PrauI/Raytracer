#ifndef OBJECT_HPP
#define OBJECT_HPP
#include <json/json.h>
#include <vector>
#include <opencv2/opencv.hpp>
#include "world.hpp"

using cv::Vec4f, cv::Vec3b, cv::Vec3f, cv::Mat;

class World;

#define Origin  Vec4f(0,0,0,1);
#define EPSILON 0.00001;

class Object;

/**
 * @brief Structure to hold intersection information.
 */
struct intersectionInfo{
    bool didHit;        /**< Indicates if an intersection occurred. */
    float t;            /**< Distance to the intersection point. */
    Vec4f position;     /**< Position of the intersection point. */
    Vec4f normal;       /**< Normal at the intersection point. */
    Vec4f dir;          /**< Direction of the ray. */
    Object* object;     /**< Pointer to the intersected object. */
};


/**
 * @brief Base class for objects in the scene.
 */
class Object{
protected:
    Vec4f position;   /**< Position of the object. */
    Vec3f ambient;    /**< Ambient color of the object. */
    Vec3f diffuse;    /**< Diffuse color of the object. */
    Vec3f specular;   /**< Specular color of the object. */
    Vec3f reflected;  /**< Reflected color of the object. */
    Vec3f refracted;  /**< Refracted color of the object. */
    float shininess;  /**< Shininess of the object. */
    float index;        /**< Index of refraction / reflection of the object. */


public:
    Mat transformationMatrix;               /**< Transformation matrix of the object. */

    /**
     * @brief Sets the position of the object from JSON input.
     * @param input JSON value containing the position.
     */
    void setPosition(Json::Value& input);

    /**
     * @brief Sets the default position of the object.
     */
    void setDefaultPosition();

    /**
     * @brief Sets the color of the object from JSON input.
     * @param color JSON value containing the color.
     */
    void setColor(Json::Value& color);

    /**
     * @brief Sets the default color of the object.
     */
    void setDefaultColor();

    /**
     * @brief Sets the index of refraction / reflection of the object from JSON input.
     * @param Jindex JSON value containing the index.
     */
    void setIndex(Json::Value& Jindex);
    Vec4f getPosition();

    /**
     * @brief Gets the ambient color of the object.
     * @return Ambient color of the object.
     */
    Vec3f getAmbient();

    /**
     * @brief Gets the diffuse color of the object.
     * @return Diffuse color of the object.
     */
    Vec3f getDiffuse();

    /**
     * @brief Gets the specular color of the object.
     * @return Specular color of the object.
     */
    Vec3f getSpecular();

 /**
     * @brief Gets the reflected color of the object.
     * @return Reflected color of the object.
     */
    Vec3f getReflected();

 /**
     * @brief Gets the refracted color of the object.
     * @return Refracted color of the object.
     */
    Vec3f getRefracted();

 /**
     * @brief Gets the shininess of the object.
     * @return Shininess of the object.
     */
    float getShininess();

 /**
     * @brief Gets the index of refraction / reflection of the object.
     * @return Index of refraction / reflection of the object.
     */
    float getIndex();

 /**
     * @brief Calculates the intersection of a ray with the object.
     * @param ray Ray to intersect with the object.
     * @param scene Scene containing the object.
     * @param closesHit Intersection information of the closest hit.
     */
    virtual void intersection(const struct Ray& ray, World* scene, intersectionInfo* closesHit) = 0;
};

/**
 * @brief Class representing a sphere in the scene.
 */
class Sphere: public Object{
private:
    float radius;  /**< Radius of the sphere. */

public:
   /**
    * @brief Constructs a Sphere object from JSON input and transformation matrix.
    * @param input JSON value containing the sphere parameters.
    * @param matrix Transformation matrix.
    */
    Sphere(Json::Value& input, Mat& matrix);

 /**
     * @brief Sets the radius of the sphere from JSON input.
     * @param input JSON value containing the radius.
     */
    virtual void intersection(const struct Ray& ray, World* scene, intersectionInfo* closesHit);
};

/**
 * @brief Class representing a half-space in the scene.
 */
class Halfspace: public Object{
private:
    Vec4f normal;  /**< Normal of the half-space. */

public:
   /**
    * @brief Constructs a Halfspace object from JSON input and transformation matrix.
    * @param input JSON value containing the half-space parameters.
    * @param matrix Transformation matrix.
    */
    Halfspace(Json::Value& input, Mat& matrix);

 /**
     * @brief Calculates the intersection of a ray with the half-space.
     * @param ray Ray to intersect with the half-space.
     * @param scene Scene containing the half-space.
     * @param closesHit Intersection information of the closest hit.
     */
    virtual void intersection(const struct Ray& ray, World* scene, intersectionInfo* closesHit);
/**
     * @brief Sets the normal of the half-space from JSON input.
     * @param input JSON value containing the normal.
     */
    void setNormal(Json::Value& input);

    /**
    * @brief Sets the default normal of the half-space.
    */
    void setDefaultNormal();
};

class Triangle: public Object{
private:
    Vec4f normal;
    Vec4f v0, v1, v2;
    Vec4f u, v;
    Vec4f uPerp, vPerp;

public:
   /**
    * @brief Constructs a Halfspace object from JSON input and transformation matrix.
    * @param input JSON value containing the half-space parameters.
    * @param matrix Transformation matrix.
    */
    Triangle(Json::Value& input, Mat& matrix);

 /**
     * @brief Calculates the intersection of a ray with the half-space.
     * @param ray Ray to intersect with the half-space.
     * @param scene Scene containing the half-space.
     * @param closesHit Intersection information of the closest hit.
     */
    virtual void intersection(const struct Ray& ray, World* scene, intersectionInfo* closesHit);
};
/**
 * @brief Calculates the length of a vector.
 * @param x Vector whose length is to be calculated.
 * @return Length of the vector.
 */
float length(Vec4f& x);


/**
 * @brief Normalizes a transformation matrix.
 * @param transformationMatrix Transformation matrix to normalize.
 */
void normalizeTransformationMatrix(cv::Mat& transformationMatrix);


Vec4f crossProduct(Vec4f& a, Vec4f& b);

#endif