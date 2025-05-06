#ifndef TERRAINGENERATOR_H_INCLUDED
#define TERRAINGENERATOR_H_INCLUDED

class Chunk;

/**
 * @class TerrainGenerator
 * @brief Abstract base class for terrain generation.
 * 
 * @details
 * The TerrainGenerator class is an abstract base class that defines the
 * interface for terrain generation in the game world. It provides methods
 * for generating terrain for a given chunk and retrieving the minimum spawn
 * height. Derived classes must implement the generateTerrainFor and
 * getMinimumSpawnHeight methods to provide specific terrain generation
 * functionality.
 */
class TerrainGenerator {
  public:
    /**
     * @brief Generates terrain for the specified chunk.
     * 
     * @param chunk The chunk to generate terrain for.
     * 
     * @details
     * This method generates terrain for the specified chunk. Derived classes
     * must implement this method to provide specific terrain generation
     * functionality.
     */
    virtual void generateTerrainFor(Chunk &chunk) = 0;
    /**
     * @brief Gets the minimum spawn height for the terrain generator.
     * 
     * @return The minimum spawn height.
     * 
     * @details
     * This method returns the minimum spawn height for the terrain generator.
     * Derived classes must implement this method to provide the minimum
     * spawn height specific to their terrain generation logic.
     */
    virtual int getMinimumSpawnHeight() const noexcept = 0;

    virtual ~TerrainGenerator() = default;
};

#endif // TERRAINGENERATOR_H_INCLUDED
