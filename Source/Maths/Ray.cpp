#include "Ray.h"

static float deg2rad(float degree) {
    return degree * 2 * glm::pi<float>() / 360.f;
}

Ray::Ray(const glm::vec3 &position, const glm::vec3 &direction)
    : m_rayStart(position)
    , m_rayEnd(position)
    , m_angles(direction)
{
    m_direction.y = glm::sin(-deg2rad(m_angles.x));
    float baseLen = glm::cos(-deg2rad(m_angles.x));
    m_direction.x = glm::sin(deg2rad(m_angles.y)) * baseLen;
    m_direction.z = -glm::cos(deg2rad(m_angles.y)) * baseLen;
    // m_direction = glm::normalize(m_direction);
}

void Ray::step(float scale)
{
    float yaw = glm::radians(m_direction.y + 90);
    float pitch = glm::radians(m_direction.x);

    auto &p = m_rayEnd;

    p.x -= glm::cos(yaw) * scale;
    p.z -= glm::sin(yaw) * scale;
    p.y -= glm::tan(pitch) * scale;
}

bool Ray::cast(World& world, float maxLength, bool face) {
    // 1. Direction vector from pitch/yaw

    // 2. Initial state
    glm::vec3 pos = m_rayStart;
    glm::ivec3 block = glm::floor(pos);
    glm::ivec3 prevBlock = block; // last non-solid block

    glm::ivec3 step;
    step.x = (m_direction.x > 0) ? 1 : -1;
    step.y = (m_direction.y > 0) ? 1 : -1;
    step.z = (m_direction.z > 0) ? 1 : -1;

    glm::vec3 offset = pos - glm::floor(pos);

    glm::vec3 first;
    first.x = (step.x > 0 ? (1.0f - offset.x) : offset.x);
    first.y = (step.y > 0 ? (1.0f - offset.y) : offset.y);
    first.z = (step.z > 0 ? (1.0f - offset.z) : offset.z);

    float lengthX = (m_direction.x != 0.0f) ? (1.0f / std::abs(m_direction.x)) : std::numeric_limits<float>::infinity();
    float lengthY = (m_direction.y != 0.0f) ? (1.0f / std::abs(m_direction.y)) : std::numeric_limits<float>::infinity();
    float lengthZ = (m_direction.z != 0.0f) ? (1.0f / std::abs(m_direction.z)) : std::numeric_limits<float>::infinity();

    float travelX = first.x * lengthX;
    float travelY = first.y * lengthY;
    float travelZ = first.z * lengthZ;

    float t = 0.0f;

    while (t <= maxLength) {
        ChunkBlock id = world.getBlock(block.x, block.y, block.z);
        if (id != BlockId::Air && id != BlockId::Water) {
            // Found solid block
            m_rayEnd = glm::vec3(face ? prevBlock : block);
            return true;
        }

        // Update previous block before stepping
        prevBlock = block;

        if (travelX < travelY && travelX < travelZ) {
            block.x += step.x;
            t = travelX;
            travelX += lengthX;
        } else if (travelY < travelZ) {
            block.y += step.y;
            t = travelY;
            travelY += lengthY;
        } else {
            block.z += step.z;
            t = travelZ;
            travelZ += lengthZ;
        }
    }

    return false; // no block hit
}

const glm::vec3 &Ray::getEnd() const
{
    return m_rayEnd;
}

float Ray::getLength() const
{
    return glm::distance(m_rayStart, m_rayEnd);
}

glm::vec3 Ray::getDirection()
{
    return m_direction;
}
