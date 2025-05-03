#ifndef FRUSTUM_H_INCLUDED
#define FRUSTUM_H_INCLUDED

#include <array>

#include "glm.h"

struct AABB;

/**
 * @struct Plane
 * @brief Class to represent a plane in 3D space.
 * 
 * @details
 * This class is used to represent a plane in 3D space using a normal vector
 * and a distance from the origin. It provides a method to calculate the
 * distance from a point to the plane.
 */
struct Plane {

    /**
    * @brief Calculates the distance from a point to the plane.
    * 
    * @param point The point to calculate the distance from.
    * @return The distance from the point to the plane.
    * 
    * @details
    * This method calculates the distance from a point to the plane using the
    * formula: distance = dot(normal, point) + distanceToOrigin.
    */
    float distanceToPoint(const glm::vec3 &point) const;

    float distanceToOrigin;
    glm::vec3 normal; // Vector3 normals
};

/**
 * @class ViewFrustum
 * @brief Class to represent a view frustum.
 * 
 * @details
 * This class is used to determine if a bounding box is within the view frustum
 * of a camera. It uses the planes of the frustum to perform this check.
 */
class ViewFrustum {
  public:
    /**
     * @brief Updates the view frustum planes based on the projection-view matrix.
     * 
     * @param mat The projection-view matrix.
     * 
     * @details
     * This method updates the planes of the view frustum based on the provided
     * projection-view matrix. It calculates the normals and distances to the origin
     * for each plane and normalizes them.
     */
    void update(const glm::mat4 &projViewMatrix) noexcept;

    /**
     * @brief Checks if a bounding box is inside the view frustum.
     * 
     * @param box The bounding box to check.
     * 
     * @return true if the box is inside the frustum, false otherwise.
     * 
     * @details
     * This method checks if a bounding box is inside the view frustum by checking
     * the distance from the box's vertices to each plane of the frustum. If any
     * plane is negative, the box is outside the frustum. If all planes are positive,
     * the box is inside the frustum. If some planes are positive and some are
     * negative, the box is partially inside the frustum.
     */
    bool isBoxInFrustum(const AABB &box) const noexcept;

  private:
    std::array<Plane, 6> m_planes;
};

#endif // FRUSTUM_H_INCLUDED
