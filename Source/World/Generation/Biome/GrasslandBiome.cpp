#include "GrasslandBiome.h"

#include "../Structures/TreeGenerator.h"

GrasslandBiome::GrasslandBiome(int seed)
    : Biome(getNoiseParameters(), 1000, 20, seed)
{
}

ChunkBlock GrasslandBiome::getTopBlock(Rand &rand) const
{
    return BlockId::Grass;
}

ChunkBlock GrasslandBiome::getUnderWaterBlock(Rand &rand) const
{
    return rand.intInRange(0, 10) > 8 ? BlockId::Dirt : BlockId::Sand;
}

ChunkBlock GrasslandBiome::getBeachBlock(Rand &rand) const
{
    return rand.intInRange(0, 10) > 2 ? BlockId::Grass : BlockId::Dirt;
}

int GrasslandBiome::getTreeType(Rand &rand, int y) const
{
    return 0;
}

NoiseParameters GrasslandBiome::getNoiseParameters()
{
    NoiseParameters heightParams;
    heightParams.octaves = 9;
    heightParams.amplitude = 85;
    heightParams.smoothness = 235;
    heightParams.heightOffset = -20;
    heightParams.roughness = 0.51;

    return heightParams;
}

ChunkBlock GrasslandBiome::getPlant(Rand &rand) const
{
    return rand.intInRange(0, 10) > 6 ? BlockId::Rose : BlockId::TallGrass;
}
