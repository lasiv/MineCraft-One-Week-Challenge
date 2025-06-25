#include "TemperateForestBiome.h"

#include "../Structures/TreeGenerator.h"

TemperateForestBiome::TemperateForestBiome(int seed)
    : Biome(getNoiseParameters(), 55, 75, seed)
{
}

ChunkBlock TemperateForestBiome::getTopBlock(Rand &rand) const
{
    return rand.intInRange(0, 10) < 8 ? BlockId::Grass : BlockId::Dirt;
}

ChunkBlock TemperateForestBiome::getUnderWaterBlock(Rand &rand) const
{
    return rand.intInRange(0, 10) > 8 ? BlockId::Dirt : BlockId::Sand;
}

int TemperateForestBiome::getTreeType(Rand &rand, int y) const
{
    return 0;
}

NoiseParameters TemperateForestBiome::getNoiseParameters()
{
    NoiseParameters heightParams;
    heightParams.octaves = 5;
    heightParams.amplitude = 100;
    heightParams.smoothness = 195;
    heightParams.heightOffset = -30;
    heightParams.roughness = 0.52;

    return heightParams;
}

ChunkBlock TemperateForestBiome::getPlant(Rand &rand) const
{
    return BlockId::TallGrass;
}
