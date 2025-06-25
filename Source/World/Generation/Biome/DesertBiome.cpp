#include "DesertBiome.h"

#include "../../WorldConstants.h"
#include "../Structures/TreeGenerator.h"

#include <iostream>

DesertBiome::DesertBiome(int seed)
    : Biome(getNoiseParameters(), 1350, 500, seed)
{
}

ChunkBlock DesertBiome::getTopBlock(Rand &rand) const
{
    return BlockId::Sand;
}

ChunkBlock DesertBiome::getUnderWaterBlock(Rand &rand) const
{
    return BlockId::Sand;
}

int DesertBiome::getTreeType(Rand &rand, int y) const
{
    if (y < WATER_LEVEL + 15) {
        if (rand.intInRange(0, 100) > 75) {
            return 1;
        }
        else {
            return 2;
        }
    }
    else {
        return 2;
    }
}

int DesertBiome::getStructure(Rand& rand, int y) const 
{
    if((y - 10) < WATER_LEVEL) return -1;
    
    if(rand.intInRange(0, 1000) < 1)  {
        return 3;
    }
    else {
        return -1;
    }
}

NoiseParameters DesertBiome::getNoiseParameters()
{
    NoiseParameters heightParams;
    heightParams.octaves = 9;
    heightParams.amplitude = 80;
    heightParams.smoothness = 335;
    heightParams.heightOffset = -7;
    heightParams.roughness = 0.56;

    return heightParams;
}

ChunkBlock DesertBiome::getPlant(Rand &rand) const
{
    return BlockId::DeadShrub;
}
