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
private:
    Vec4f position;     /**< Position of the object. */
    Vec3f ambient;      /**< Ambient color of the object. */
    Vec3f diffuse;      /**< Diffuse color of the object. */
    Vec3f specular;     /**< Specular color of the object. */
    Vec3f reflected;    /**< Reflected color of the object. */
    Vec3f refracted;    /**< Refracted color of the object. */
    float shininess;    /**< Shininess coefficient of the object. */


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
     * @brief Gets the position of the object.
     * @return Position of the object.
     */
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
     * @brief Gets the shininess coefficient of the object.
     * @return Shininess coefficient of the object.
     */
    float getShininess();

    /**
     * @brief Pure virtual function to calculate the intersection of a ray with the object.
     * @param S Starting point of the ray.
     * @param d Direction of the ray.
     * @param scene Pointer to the scene (instance of world class).
     * @return Intersection information.
     */
    virtual struct intersectionInfo* intersection(const Vec4f& S, const Vec4f& d, World* scene) = 0;
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
    * @brief Calculates the intersection of a ray with the sphere.
    * @param S Starting point of the ray.
    * @param d Direction of the ray.
    * @param scene Pointer to the scene (instance of world class).
    * @return Intersection information.
    */
    struct intersectionInfo* intersection(const Vec4f& S, const Vec4f& d, World* scene);
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
    * @param S Starting point of the ray.
    * @param d Direction of the ray.
    * @param scene Pointer to the scene.
    * @return Intersection information.
    */
    struct intersectionInfo* intersection(const Vec4f& S, const Vec4f& d, World* scene);

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

/**
 * @brief Calculates the length of a vector.
 * @param x Vector whose length is to be calculated.
 * @return Length of the vector.
 */
float length(Vec4f& x);

/**
 * @brief Calculates the scalar product of two vectors.
 * @param a First vector.
 * @param b Second vector.
 * @return Scalar product of the two vectors.
 */
float scalarProduct(const Vec4f& a, const Vec4f& b);




#endif