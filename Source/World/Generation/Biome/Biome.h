#ifndef BIOME_H_INCLUDED
#define BIOME_H_INCLUDED

#include "../../../Maths/NoiseGenerator.h"
#include "../../../Util/Random.h"
#include "../../Block/ChunkBlock.h"

using Rand = Random<std::minstd_rand>;

class Chunk;

/**
 * @class Biome
 * @brief Base class for different biomes in the game world.
 * 
 * @details
 * The Biome class serves as a base class for different biomes in the game world.
 * It provides methods to generate terrain features such as plants, trees,
 * top blocks, and underwater blocks. The class uses noise generation to
 * determine the height of the terrain and the characteristics of the biome.
 * It also provides methods to set the frequency of trees and plants in the biome.
 */
struct Biome {
  public:
    Biome(const NoiseParameters &parameters, int treeFreq, int plantFreq,
          int seed);
    virtual ~Biome() = default;

    virtual ChunkBlock getPlant(Rand &rand) const = 0;
    virtual ChunkBlock getTopBlock(Rand &rand) const = 0;
    virtual ChunkBlock getUnderWaterBlock(Rand &rand) const = 0;
    virtual ChunkBlock getBeachBlock(Rand &rand) const;
    virtual int getTreeType(Rand &rand, int y) const = 0;

    int getHeight(int x, int z, int chunkX, int chunkZ) const;
    int getTreeFrequency() const noexcept;
    int getPlantFrequency() const noexcept;

  protected:
    virtual NoiseParameters getNoiseParameters() = 0;

  private:
    NoiseGenerator m_heightGenerator;
    int m_treeFreq;
    int m_plantFreq;
};

#endif // BIOME_H_INCLUDED
