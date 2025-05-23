#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include "Maths/Matrix.h"
#include "Physics/AABB.h"

/**
 * @struct Entity
 * @brief Represents a 3D entity in the game world.
 * 
 * @details
 * The Entity struct encapsulates the position, rotation, velocity, and bounding box
 * of an entity. It is used as a base class for player and other game objects.
 */
struct Entity {
    Entity()
        : box({0.f, 0.f, 0.f})
        , position(glm::vec3(0.f))
        , rotation(glm::vec3(0.f))
        , velocity(glm::vec3(0.f))
    {
    }

    Entity(const glm::vec3 &pos, const glm::vec3 &rot)
        : position(pos)
        , rotation(rot)
        , box({0, 0, 0})
        , velocity(glm::vec3(0.f))
    {
    }
    Entity(const glm::vec3 &pos, const glm::vec3 &rot, const glm::vec3 &box)
        : position(pos)
        , rotation(rot)
        , box(box)
        , velocity(glm::vec3(0.f))
    {
    }

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 velocity;

    AABB box;
};

#endif // ENTITY_H_INCLUDED
