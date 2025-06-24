#ifndef CLASSICOVERWORLDGENERATOR_H_INCLUDED
#define CLASSICOVERWORLDGENERATOR_H_INCLUDED

#include "TerrainGenerator.h"

#include "../../../Util/Array2D.h"
#include "../../../Util/Random.h"

#include "../../../Maths/NoiseGenerator.h"
#include "../../WorldConstants.h"

#include "../Biome/DesertBiome.h"
#include "../Biome/GrasslandBiome.h"
#include "../Biome/LightForest.h"
#include "../Biome/OceanBiome.h"
#include "../Biome/TemperateForestBiome.h"
#include <SFML/System/Vector3.hpp>
#include <vector>
#include "../Structures/Structure.h"

class Chunk;

/**
 * @class ClassicOverWorldGenerator
 * @brief Generates terrain for the classic overworld.
 * 
 * @details
 * The ClassicOverWorldGenerator class is responsible for generating terrain
 * for the classic overworld using perlin noise and recognizable Minecraft
 * parameters. It creates a height map and biome map for the terrain, and
 * generates blocks based on the height and biome information. The generator
 * uses various biome classes to define the characteristics of different
 * biomes, such as grassland, temperate forest, desert, ocean, and light
 * forest. The generator also provides methods to set up noise parameters
 * and generate terrain for a given chunk.
 */
class ClassicOverWorldGenerator : public TerrainGenerator {
  public:
    ClassicOverWorldGenerator();

    /**
     * @brief Generates terrain for the specified chunk.
     * 
     * @param chunk The chunk to generate terrain for.
     * 
     * @details
     * This method generates terrain for the specified chunk by creating a height
     * map and biome map. It uses the noise generator to create a height value
     * based on the chunk location. The method sets the blocks in the chunk
     * according to the generated height and biome information. It also handles
     * the placement of trees and plants based on the biome and height values.
     */
    void generateTerrainFor(Chunk &chunk) override;
    
    /**
     * @brief Gets the minimum spawn height for the terrain generator.
     * 
     * @return The minimum spawn height.
     * 
     * @details
     * This method returns the minimum spawn height for the terrain generator.
     * The spawn height is set to a constant value defined by WATER_LEVEL.
     */
    int getMinimumSpawnHeight() const noexcept override;
    void check_integrity();

  private:
    
    static void setUpNoise();

    /**
     * @brief Sets the blocks in the chunk based on the height map and biome map.
     * 
     * @param maxHeight The maximum height value for the chunk.
     * 
     * @details
     * This method sets the blocks in the chunk based on the height map and
     * biome map. It iterates through the chunk coordinates and sets the
     * appropriate block type based on the height and biome values. It also
     * handles the placement of trees and plants based on the biome and
     * height values. The method uses the setBlock method to set the block
     * type for each coordinate. It also handles the placement of water
     * blocks and dirt blocks based on the height values. The method
     * ensures that the blocks are set within the bounds of the chunk size.
     * It uses the getBiome method to retrieve the biome for each coordinate
     * and sets the block type accordingly. The method also handles the
     * placement of trees and plants based on the biome and height values.
     * It uses the getTree method from the biome class to generate
     * trees and plants.
     */
    void setBlocks(int maxHeight);

    /**
     * @brief Gets the height in the specified area.
     * 
     * @param xMin The minimum x-coordinate of the area.
     * @param zMin The minimum z-coordinate of the area.
     * @param xMax The maximum x-coordinate of the area.
     * @param zMax The maximum z-coordinate of the area.
     * 
     * @details
     * This method calculates the height in the specified area by using
     * bilinear interpolation. It retrieves the height values at the corners
     * of the area and uses them to calculate the height at each point in
     * the area. The calculated height values are stored in the height map.
     * The method ensures that the height values are within the bounds of
     * the chunk size.
     */
    void getHeightIn(int xMin, int zMin, int xMax, int zMax);
    
    /**
     * @brief Gets the height map for the chunk.
     * 
     * @details
     * This method generates the height map for the chunk by dividing the
     * chunk into four quadrants and calculating the height for each quadrant.
     * It uses the getHeightIn method to calculate the height values for
     * each quadrant. The height values are stored in the height map.
     */
    void getHeightMap();
    
    /**
     * @brief Gets the biome map for the chunk.
     * 
     * @details
     * This method generates the biome map for the chunk by using the
     * biome noise generator. It iterates through the chunk coordinates and
     * retrieves the height value for each coordinate. The height values
     * are used to determine the biome type for each coordinate. The
     * biome values are stored in the biome map.
     */
    void getBiomeMap();

    /**<int>
     * @brief Gets the biome for the specified coordinates.
     * 
     * @param x The x-coordinate of the block.
     * @param z The z-coordinate of the block.
     * 
     * @return The biome at the specified coordinates.
     * 
     * @details
     * This method retrieves the biome for the specified coordinates by
     * checking the biome value in the biome map. It uses a series of
     * conditional statements to determine the biome type based on the
     * biome value. The method returns a reference to the corresponding
     * biome object. The biome types are defined by the biome value ranges.
     * The method ensures that the biome value is within the bounds of the
     * biome map. It uses the getBiome method to retrieve the biome for
     * each coordinate and returns the corresponding biome object.
     */
    const Biome &getBiome(int x, int z) const;

    void getLargeVegetation(int chunkX, int chunkZ, std::vector<std::pair<sf::Vector3i, Structure*>>& structures);

    Array2D<int, 3*CHUNK_SIZE> m_heightMap;
    Array2D<int, 3*CHUNK_SIZE + 1> m_biomeMap;

    Random<std::minstd_rand> m_random;

    static NoiseGenerator m_biomeNoiseGen;

    std::vector<std::vector<Structure>> structures;

    GrasslandBiome m_grassBiome;
    TemperateForestBiome m_temperateForest;
    DesertBiome m_desertBiome;
    OceanBiome m_oceanBiome;
    LightForest m_lightForest;

    Chunk *m_pChunk = nullptr;
};

#endif // CLASSICOVERWORLDGENERATOR_H_INCLUDED
