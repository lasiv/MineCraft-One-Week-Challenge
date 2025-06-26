#ifndef RAY_H_INCLUDED
#define RAY_H_INCLUDED

#include "../World/World.h"
#include "glm.h"

/**
 * @class Ray
 * @brief Class to represent a ray in 3D space.
 * 
 * @details
 * This class is used to represent a ray in 3D space, defined by a starting
 * position and a direction. It provides methods to step the ray in the
 * specified direction and to get the end position of the ray. The ray can
 * be used for various purposes, such as raycasting, collision detection,
 * and line-of-sight calculations.
 */
class Ray {
  public:
    Ray(const glm::vec3 &position, const glm::vec3 &direction);

    /**
     * @brief Steps the ray in the specified direction.
     * 
     * @param scale The distance to step the ray.
     * 
     * @details
     * This method steps the ray in the specified direction by a given scale.
     * It calculates the yaw and pitch angles based on the direction vector and
     * updates the end position of the ray accordingly.
     */
    void step(float scale);

    bool cast(World& world, float maxLength, bool face);

    /**
     * @brief Gets the end position of the ray.
     * 
     * @return The end position of the ray.
     * 
     * @details
     * This method returns the end position of the ray as a constant reference
     * to a glm::vec3 object. The end position is updated when the ray is stepped.
     */
    const glm::vec3 &getEnd() const;

    /**
     * @brief Gets the length of the ray.
     * 
     * @return The length of the ray.
     * 
     * @details
     * This method calculates and returns the length of the ray by computing
     * the distance between the starting and ending positions of the ray.
     */
    float getLength() const;

    // returns direction as a vector
    glm::vec3 getDirection();

  private:
    glm::vec3 m_rayStart;
    glm::vec3 m_rayEnd;
    glm::vec3 m_direction;
    glm::vec3 m_angles;
};

#endif // RAY_H_INCLUDED
