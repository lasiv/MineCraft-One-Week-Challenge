#include "LightForest.h"

#include "../Structures/TreeGenerator.h"

LightForest::LightForest(int seed)
    : Biome(getNoiseParameters(), 60, 80, seed)
{
}

ChunkBlock LightForest::getTopBlock(Rand &rand) const
{
    return BlockId::Grass;
}

ChunkBlock LightForest::getUnderWaterBlock(Rand &rand) const
{
    return rand.intInRange(0, 10) > 9 ? BlockId::Sand : BlockId::Dirt;
}

int LightForest::getTreeType(Rand &rand, int y) const
{
    return 0;
}

NoiseParameters LightForest::getNoiseParameters()
{
    NoiseParameters heightParams;
    heightParams.octaves = 5;
    heightParams.amplitude = 100;
    heightParams.smoothness = 195; // 195
    heightParams.heightOffset = -32;
    heightParams.roughness = 0.52;

    return heightParams;
}

ChunkBlock LightForest::getPlant(Rand &rand) const
{
    return rand.intInRange(0, 10) > 8 ? BlockId::Rose : BlockId::TallGrass;
}
