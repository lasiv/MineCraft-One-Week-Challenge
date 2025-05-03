#ifndef VECTOR2XZ_H_INCLUDED
#define VECTOR2XZ_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <functional>

/**
 * @struct VectorXZ
 * 
 * @brief Structure to represent a 2D vector in the XZ plane.
 * 
 * @details
 * This structure represents a 2D vector in the XZ plane, where the x coordinate
 * represents the horizontal axis and the z coordinate represents the vertical
 * axis. It is used for various calculations and operations involving 2D vectors
 * in the XZ plane, such as noise generation and terrain calculations.
 */
struct VectorXZ {
    int x, z;
};

/**
 * @brief Overloaded equality operator for VectorXZ.
 * 
 * @param left The left operand of the equality operator.
 * @param right The right operand of the equality operator.
 * 
 * @return true if the two vectors are equal, false otherwise.
 * 
 * @details
 * This function overloads the equality operator for the VectorXZ structure.
 * It compares the x and z coordinates of the two vectors and returns true
 * if they are equal, and false otherwise.
 */
bool operator==(const VectorXZ &left, const VectorXZ &right) noexcept;

namespace std {

/**
 * @brief Specialization of std::hash for VectorXZ.
 * 
 * @details
 * This specialization of std::hash provides a hash function for the VectorXZ
 * structure. It combines the hashes of the x and z coordinates to create a
 * unique hash value for the vector.
 */
template <> struct hash<VectorXZ> {
    size_t operator()(const VectorXZ &vect) const noexcept
    {
        std::hash<decltype(vect.x)> hasher;

        auto hash1 = hasher(vect.x);
        auto hash2 = hasher(vect.z);

        return std::hash<decltype(vect.x)>{}((hash1 ^ hash2) >> 2);
    }
};
} // namespace std

namespace std {

/**
 * @brief Specialization of std::hash for sf::Vector3i.
 * 
 * @details
 * This specialization of std::hash provides a hash function for the sf::Vector3i
 * structure. It combines the hashes of the x, y, and z coordinates to create a
 * unique hash value for the vector.
 */
template <> struct hash<sf::Vector3i> {
    size_t operator()(const sf::Vector3i &vect) const noexcept
    {
        std::hash<decltype(vect.x)> hasher;

        auto hash1 = hasher(vect.x);
        auto hash2 = hasher(vect.y);
        auto hash3 = hasher(vect.z);

        return std::hash<decltype(vect.x)>{}(
            (hash1 ^ (hash2 << hash3) ^ hash3));
    }
};
} // namespace std

#endif // VECTOR2XZ_H_INCLUDED
