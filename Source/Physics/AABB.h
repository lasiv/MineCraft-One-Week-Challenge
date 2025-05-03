#ifndef AABB_H_INCLUDED
#define AABB_H_INCLUDED

#include "../Maths/glm.h"

/**
 * @struct AABB
 * 
 * @brief Structure to represent an Axis-Aligned Bounding Box (AABB).
 * 
 * @details
 * This structure represents an Axis-Aligned Bounding Box (AABB) in 3D space.
 * It is defined by its position and dimensions. The AABB is used for collision
 * detection and bounding volume calculations. The position represents the
 * center of the AABB, and the dimensions represent the size of the box in
 * each axis (x, y, z). The AABB can be updated with a new position and
 * provides methods to calculate the vertices of the box based on a given
 * normal vector.
 */
struct AABB {
    AABB(const glm::vec3 &dim)
        : dimensions(dim)
    {
    }

    /**
     * @brief Updates the position of the AABB.
     * 
     * @param location The new position of the AABB.
     * 
     * @details
     * This method updates the position of the AABB to the specified location.
     * The position represents the center of the AABB in 3D space. The dimensions
     * of the AABB remain unchanged. This method is used to move the AABB to a
     * new location in the world space.
     */
    void update(const glm::vec3 &location)
    {
        position = location;
    }

    /**
     * @brief Gets the vertex opposite to the given normal vector.
     * 
     * @param normal The normal vector used to determine the reference direction.
     * @return The vertex on the AABB opposite to the direction of the normal.
     * 
     * @details
     * This method returns the vertex of the axis-aligned bounding box (AABB)
     * that lies in the direction opposite to the specified normal vector.
     * 
     * For each axis (x, y, z), if the component of the normal is negative,
     * the corresponding dimension is added to the position to reach the opposite corner.
     * 
     * This is typically used in collision detection for selecting the
     * farthest vertex away from a separating axis or normal vector.
     */
    glm::vec3 getVN(const glm::vec3 &normal) const
    {
        glm::vec3 res = position;

        if (normal.x < 0) {
            res.x += dimensions.x;
        }
        if (normal.y < 0) {
            res.y += dimensions.y;
        }
        if (normal.z < 0) {
            res.z += dimensions.z;
        }

        return res;
    }

    /**
     * @brief Gets the vertex in the direction of the given normal vector.
     * 
     * @param normal The normal vector used to determine the reference direction.
     * @return The vertex on the AABB aligned with the direction of the normal.
     * 
     * @details
     * This method returns the vertex of the axis-aligned bounding box (AABB)
     * that lies in the same direction as the specified normal vector.
     * 
     * For each axis (x, y, z), if the component of the normal is positive,
     * the corresponding dimension is added to the position to reach that corner.
     * 
     * This is useful in collision response or broad-phase culling when testing
     * support points in a specific direction.
     */
    glm::vec3 getVP(const glm::vec3 &normal) const
    {
        glm::vec3 res = position;

        if (normal.x > 0) {
            res.x += dimensions.x;
        }
        if (normal.y > 0) {
            res.y += dimensions.y;
        }
        if (normal.z > 0) {
            res.z += dimensions.z;
        }

        return res;
    }

    glm::vec3 position;
    const glm::vec3 dimensions;
};

#endif // AABB_H_INCLUDED
