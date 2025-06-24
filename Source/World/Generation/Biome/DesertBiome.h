#ifndef DESERTBIOME_H_INCLUDED
#define DESERTBIOME_H_INCLUDED

#include "Biome.h"

class DesertBiome : public Biome {
  public:
    DesertBiome(int seed);

    ChunkBlock getPlant(Rand &rand) const override;
    ChunkBlock getTopBlock(Rand &rand) const override;
    ChunkBlock getUnderWaterBlock(Rand &rand) const override;
    int getTreeType(Rand &rand, int y) const override;

  private:
    NoiseParameters getNoiseParameters() override;
};

#endif // DESERTBIOME_H_INCLUDED
