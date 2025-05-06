#ifndef SUPERFLATGENERATOR_H_INCLUDED
#define SUPERFLATGENERATOR_H_INCLUDED

#include "TerrainGenerator.h"

/**
 * @class SuperFlatGenerator
 * @brief Generates a super flat terrain.
 * 
 * @details
 * The SuperFlatGenerator class is responsible for generating a super flat
 * terrain for the game world. It inherits from the TerrainGenerator class
 * and implements the generateTerrainFor method to create a flat terrain
 * with a specified block type. The generator can be customized to use
 * different block types for the flat terrain. The class also provides
 * a method to get the minimum spawn height for the terrain generator.
 * The minimum spawn height is set to a constant value defined by
 * WATER_LEVEL. The generator is designed to be simple and efficient,
 * making it suitable for creating flat worlds or testing purposes.
 */
class SuperFlatGenerator : public TerrainGenerator {
  public:
    /**
     * @brief Generates terrain for the specified chunk.
     * 
     * @param chunk The chunk to generate terrain for.
     * 
     * @details
     * This method generates terrain for the specified chunk by creating a flat
     * terrain with a specified block type. It sets the blocks in the chunk
     * according to the super flat terrain generation rules.
     */
    void generateTerrainFor(Chunk &chunk) override;

    /**
     * @brief Gets the minimum spawn height for the super flat terrain generator.
     * 
     * @return The minimum spawn height, which is set to 1 for super flat terrain.
     * 
     * @details
     * This method returns the minimum spawn height for the super flat terrain
     * generator. The spawn height is set to a constant value of 1, which is
     * suitable for super flat worlds.
     */
    int getMinimumSpawnHeight() const noexcept override;
};

#endif // SUPERFLATGENERATOR_H_INCLUDED
