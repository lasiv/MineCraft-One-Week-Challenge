#ifndef TREEGENERATOR_H_INCLUDED
#define TREEGENERATOR_H_INCLUDED

#include "../../../Util/Random.h"

class Chunk;


/**
 * @brief Generates a tree in the specified chunk at the given coordinates.
 * 
 * @param chunk The chunk where the tree will be generated.
 * @param rand The random number generator used for tree generation.
 * @param x The x-coordinate of the tree's base.
 * @param y The y-coordinate of the tree's base.
 * @param z The z-coordinate of the tree's base.
 * 
 * @details
 * This function generates a tree in the specified chunk at the given coordinates.  
 */
void makeOakTree(Chunk &chunk, Random<std::minstd_rand> &rand, int x, int y, int z);
/**
 * @brief Generates a palm tree in the specified chunk at the given coordinates.
 * 
 * @param chunk The chunk where the palm tree will be generated.
 * @param rand The random number generator used for palm tree generation.
 * @param x The x-coordinate of the palm tree's base.
 * @param y The y-coordinate of the palm tree's base.
 * @param z The z-coordinate of the palm tree's base.
 * 
 * @details
 * This function generates a palm tree in the specified chunk at the given coordinates.
 */
void makePalmTree(Chunk &chunk, Random<std::minstd_rand> &rand, int x, int y, int z);
/**
 * @brief Generates a cactus in the specified chunk at the given coordinates.
 * 
 * @param chunk The chunk where the cactus will be generated.
 * @param rand The random number generator used for cactus generation.
 * @param x The x-coordinate of the cactus's base.
 * @param y The y-coordinate of the cactus's base.
 * @param z The z-coordinate of the cactus's base.
 * 
 * @details
 * This function generates a cactus in the specified chunk at the given coordinates.
 * The cactus is generated with a random height and may have additional branches.
 */
void makeCactus(Chunk &chunk, Random<std::minstd_rand> &rand, int x, int y, int z);

#endif // TREEGENERATOR_H_INCLUDED
