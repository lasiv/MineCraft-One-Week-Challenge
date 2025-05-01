#ifndef BLOCKID_H_INCLUDED
#define BLOCKID_H_INCLUDED

#include <cstdint>

using Block_t = uint8_t;

/**
 * @enum BlockId
 * @brief Enumeration of block IDs used in the game.
 *
 * @details
 * This enumeration defines the unique IDs for different block types in the game world.
 * Each block type is represented by a specific ID, which is used for rendering and
 * interaction within the game.
 */
enum class BlockId : Block_t {
    Air = 0,
    Grass = 1,
    Dirt = 2,
    Stone = 3,
    OakBark = 4,
    OakLeaf = 5,
    Sand = 6,
    Water = 7,
    Cactus = 8,
    Rose = 9,
    TallGrass = 10,
    DeadShrub = 11,

    NUM_TYPES
};

#endif // BLOCKID_H_INCLUDED
